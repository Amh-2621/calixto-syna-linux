// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2025 Synaptics Incorporated */

/**
 * VPP Boot Configuration Module
 *
 * This module handles reading boot configuration files to update VPP display
 * parameters. It provides Linux kernel equivalent functionality to U-Boot's
 * read_boot_file() function.
 */

#include "vpp_api.h"
#include "vpp_config.h"

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/blkdev.h>

/* Boot configuration file definitions */
#define VPP_BOOT_CONFIG_FILE	"/boot/res.txt"
#define VPP_RES_CONFIG_KEY		"DISP1_RESID="
#define VPP_DISPLAY_MODE_CONFIG_KEY	"DISP1_MODE="
#define VPP_DISP1_BIT_DEPTH_CONFIG_KEY	"DISP1_BPP="
#define VPP_DISP1_COLORFORMAT_CONFIG_KEY	"DISP1_COLORFORMAT="
#define VPP_CONFIG_BUF_SIZE		1024

/* Configuration variable structure for table-driven parsing */
typedef struct {
	int *var_ptr;
	char *var_name;
	char var_desc[64];
} cfg_vars_type;

/**
 * parse_cfg_int - Parse integer configuration value from buffer
 * @buf: Buffer containing configuration data
 * @key: Configuration key to search for (e.g., "RES=")
 * @val: Pointer to store parsed integer value
 *
 * Returns: 0 on success, -ENOENT if key not found, negative error code on failure
 */
static int parse_cfg_int(char *buf, char *key, int *val)
{
	char *p;
	long parsed_val;
	int ret;

	if (!buf || !key || !val) {
		pr_err("VPP boot config: Invalid parameters to parse_cfg_int\n");
		return -EINVAL;
	}

	p = strstr(buf, key);
	if (!p) {
		/* Key not found - this is not an error, parameter is optional */
		return -ENOENT;
	}

	ret = kstrtol(p + strlen(key), 10, &parsed_val);
	if (ret) {
		pr_err("VPP boot config: Invalid format for key '%s' (error %d)\n", key, ret);
		return ret;
	}

	*val = parsed_val;
	return 0;
}

/**
 * MV_VPP_ReadBootConfig - Interface to read boot configuration parameters
 * @config: Pointer to vpp_config_params structure to update
 * Returns: 0 on success, negative error code on failure
 */
int MV_VPP_ReadBootConfig(vpp_config_params *config)
{
	struct file *fp;
	char *buf;
	loff_t pos;
	ssize_t ret;
	int val;
	int i;

	/* Configuration variables table */
	cfg_vars_type cfg_vars[] = {
		{&config->disp1_res_id, VPP_RES_CONFIG_KEY, "Resolution"},
		{&config->display_mode, VPP_DISPLAY_MODE_CONFIG_KEY, "Display mode"},
		{&config->disp1_bit_depth, VPP_DISP1_BIT_DEPTH_CONFIG_KEY, "BIT_DEPTH"},
		{&config->disp1_colorformat, VPP_DISP1_COLORFORMAT_CONFIG_KEY, "COLORFORMAT"},
	};
	int cfg_count = sizeof(cfg_vars) / sizeof(cfg_vars[0]);

	if (!config) {
		pr_err("VPP boot config: Invalid config parameter\n");
		return -EINVAL;
	}

	/* Allocate buffer for file content */
	buf = kzalloc(VPP_CONFIG_BUF_SIZE, GFP_KERNEL);
	if (!buf) {
		pr_err("VPP boot config: Failed to allocate buffer\n");
		return -ENOMEM;
	}

	/* Open boot configuration file */
	fp = filp_open(VPP_BOOT_CONFIG_FILE, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		pr_warn("VPP boot config: Cannot open %s (error %ld), using defaults\n",
			VPP_BOOT_CONFIG_FILE, PTR_ERR(fp));
		kfree(buf);
		return PTR_ERR(fp);
	}

	/* Read file content */
	ret = kernel_read(fp, buf, VPP_CONFIG_BUF_SIZE - 1, &pos);
	if (ret < 0) {
		pr_err("VPP boot config: Failed to read %s (error %zd)\n",
			VPP_BOOT_CONFIG_FILE, ret);
		filp_close(fp, NULL);
		kfree(buf);
		return ret;
	}

	/* Null-terminate the buffer */
	buf[ret] = '\0';

	/* Close file */
	filp_close(fp, NULL);

	/* Parse all configuration parameters using table-driven approach */
	for (i = 0; i < cfg_count; i++) {
		ret = parse_cfg_int(buf, cfg_vars[i].var_name, &val);
		if (!ret) {
			*(cfg_vars[i].var_ptr) = val;
			pr_info("VPP boot config: %s set to %d\n", cfg_vars[i].var_desc, val);
		} else if (ret != -ENOENT) {
			/* Error other than key not found */
			goto error_cleanup;
		}
	}

	kfree(buf);
	return 0;

error_cleanup:
	kfree(buf);
	return ret;
}

/**
 * MV_VPP_WriteBootConfig - Enhanced interface to write multiple boot configuration parameters
 * @config: Pointer to vpp_config_params structure containing parameters to write
 * Returns: 0 on success, negative error code on failure
 */
int MV_VPP_WriteBootConfig(vpp_config_params *config)
{
	struct file *fp;
	char *buf;
	loff_t pos = 0;
	ssize_t ret;
	int bytes_written = 0;
	int i, len;

	/* Configuration variables table */
	cfg_vars_type cfg_vars[] = {
		{&config->disp1_res_id, VPP_RES_CONFIG_KEY, "Resolution"},
		{&config->display_mode, VPP_DISPLAY_MODE_CONFIG_KEY, "Display mode"},
		{&config->disp1_bit_depth, VPP_DISP1_BIT_DEPTH_CONFIG_KEY, "BIT_DEPTH"},
		{&config->disp1_colorformat, VPP_DISP1_COLORFORMAT_CONFIG_KEY, "COLORFORMAT"},
	};
	int cfg_count = sizeof(cfg_vars) / sizeof(cfg_vars[0]);

	if (!config) {
		pr_err("VPP boot config: Invalid config parameter\n");
		return -EINVAL;
	}

	/* Allocate buffer for file content */
	buf = kzalloc(VPP_CONFIG_BUF_SIZE, GFP_KERNEL);
	if (!buf) {
		pr_err("VPP boot config: Failed to allocate write buffer\n");
		return -ENOMEM;
	}

	/* Format the configuration content using table-driven approach */
	for (i = 0; i < cfg_count; i++) {
		len = snprintf(buf + bytes_written, VPP_CONFIG_BUF_SIZE - bytes_written,
				   "%s%d\n", cfg_vars[i].var_name, *(cfg_vars[i].var_ptr));
		if (len < 0 || bytes_written + len >= VPP_CONFIG_BUF_SIZE) {
			pr_err("VPP boot config: Configuration string too long\n");
			kfree(buf);
			return -EINVAL;
		}
		bytes_written += len;
	}

	/* Open boot configuration file for writing */
	fp = filp_open(VPP_BOOT_CONFIG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (IS_ERR(fp)) {
		pr_err("VPP boot config: Cannot open %s for writing (error %ld)\n",
		       VPP_BOOT_CONFIG_FILE, PTR_ERR(fp));
		kfree(buf);
		return PTR_ERR(fp);
	}

	/* Write configuration to file */
	ret = kernel_write(fp, buf, bytes_written, &pos);
	if (ret != bytes_written) {
		pr_err("VPP boot config: Failed to write to %s (wrote %zd of %d bytes)\n",
		       VPP_BOOT_CONFIG_FILE, ret, bytes_written);
		filp_close(fp, NULL);
		kfree(buf);
		return (ret < 0) ? ret : -EIO;
	}

	/* Sync block device to ensure persistence */
	vfs_fsync(fp, 0);
	if (fp->f_inode && fp->f_inode->i_sb && fp->f_inode->i_sb->s_bdev) {
		sync_blockdev(fp->f_inode->i_sb->s_bdev);
		pr_info("VPP boot config: Block device sync completed for %s\n",
			VPP_BOOT_CONFIG_FILE);
	} else {
		pr_info("VPP boot config: Unable to sync block device for %s\n",
			VPP_BOOT_CONFIG_FILE);
	}

	/* Close file */
	filp_close(fp, NULL);
	kfree(buf);

	pr_info("VPP boot config: Successfully updated %s with resolution %u, mode %u, bpp %u, format %u\n",
		VPP_BOOT_CONFIG_FILE, config->disp1_res_id, config->display_mode,
		config->disp1_bit_depth, config->disp1_colorformat);

	return 0;
}