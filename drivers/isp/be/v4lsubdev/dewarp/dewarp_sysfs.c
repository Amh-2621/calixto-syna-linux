
// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <media/videobuf2-v4l2.h>
#include <linux/v4l2-mediabus.h>
#include <media/v4l2-subdev.h>
#include <linux/platform_device.h>
#include "dewarp_subdev.h"

static struct class *dewarp_class;
static struct device *dewarp_class_dev;

#define SYSFS_SHOW(name, expr) \
static ssize_t name##_show(struct device *dev, \
                struct device_attribute *attr, char *buf) \
{ \
    struct dewarp_subdev_dev *dw_dev = dev_get_drvdata(dev); \
    return sprintf(buf, "0x%x\n", (expr)); \
}

#define SYSFS_STORE(name, expr) \
static ssize_t name##_store(struct device *dev, \
                struct device_attribute *attr, const char *buf, size_t count) \
{ \
    struct dewarp_subdev_dev *dw_dev = dev_get_drvdata(dev); \
    unsigned int val; \
    if (kstrtouint(buf, 0, &val) == 0) \
        (expr) = val; \
    return count; \
}

#define SYSFS_ATTR(name, expr) \
    SYSFS_SHOW(name, expr) \
    SYSFS_STORE(name, expr) \
    static DEVICE_ATTR_RW(name)


SYSFS_ATTR(cisHwin, dw_dev->lut_cfg.hwParams.cisHwin);
SYSFS_ATTR(cisVwin, dw_dev->lut_cfg.hwParams.cisVwin);
SYSFS_ATTR(disHwin, dw_dev->lut_cfg.hwParams.disHwin);
SYSFS_ATTR(disVwin, dw_dev->lut_cfg.hwParams.disVwin);
SYSFS_ATTR(disHgrid, dw_dev->lut_cfg.hwParams.disHgrid);
SYSFS_ATTR(disVgrid, dw_dev->lut_cfg.hwParams.disVgrid);
SYSFS_ATTR(cisMatSize, dw_dev->lut_cfg.derivedParam.cisMatSize);
SYSFS_ATTR(cisBlkHtotal, dw_dev->lut_cfg.derivedParam.cisBlkHtotal);
SYSFS_ATTR(cisBlkVtotal, dw_dev->lut_cfg.derivedParam.cisBlkVtotal);
SYSFS_ATTR(inWidth, dw_dev->lut_cfg.frameParam.in_frame.win.width);
SYSFS_ATTR(outWidth, dw_dev->lut_cfg.frameParam.out_frame.win.width);
SYSFS_ATTR(inHeight, dw_dev->lut_cfg.frameParam.in_frame.win.height);
SYSFS_ATTR(outHeight, dw_dev->lut_cfg.frameParam.out_frame.win.height);
SYSFS_ATTR(maxDisBlocks, dw_dev->lut_cfg.derivedParam.maxDisBlocks);
SYSFS_ATTR(disV4H2scan, dw_dev->lut_cfg.derivedParam.disV4H2scan);
SYSFS_ATTR(biCubicOff, dw_dev->lut_cfg.hwParams.biCubicOff);
SYSFS_ATTR(cisBsize, dw_dev->lut_cfg.derivedParam.cisBsize);
SYSFS_ATTR(limitCbuff, dw_dev->lut_cfg.derivedParam.limitCbuff);
SYSFS_ATTR(disLumaTiles, dw_dev->lut_cfg.derivedParam.disLumaTiles);
SYSFS_ATTR(disChromaTiles, dw_dev->lut_cfg.derivedParam.disChromaTiles);
SYSFS_ATTR(disBlkSize, dw_dev->lut_cfg.derivedParam.disBlkSize);
SYSFS_ATTR(disBlkChromaSize, dw_dev->lut_cfg.derivedParam.disBlkChromaSize);
SYSFS_ATTR(blankLuma, dw_dev->lut_cfg.derivedParam.blankLuma);
SYSFS_ATTR(blankChroma, dw_dev->lut_cfg.derivedParam.blankChroma);
SYSFS_ATTR(validDisBlks, dw_dev->lut_cfg.derivedParam.validDisBlks);
SYSFS_ATTR(inBitDepth, dw_dev->lut_cfg.frameParam.in_frame.bit_depth);
SYSFS_ATTR(outBitDepth, dw_dev->lut_cfg.frameParam.out_frame.bit_depth);
SYSFS_ATTR(inFmt, dw_dev->lut_cfg.frameParam.in_frame.fmt);
SYSFS_ATTR(outFmt, dw_dev->lut_cfg.frameParam.out_frame.fmt);

static struct attribute *dewarp_attrs[] = {
    &dev_attr_cisHwin.attr,
    &dev_attr_cisVwin.attr,
    &dev_attr_disHwin.attr,
    &dev_attr_disVwin.attr,
    &dev_attr_disHgrid.attr,
    &dev_attr_disVgrid.attr,
    &dev_attr_cisMatSize.attr,
    &dev_attr_cisBlkHtotal.attr,
    &dev_attr_cisBlkVtotal.attr,
    &dev_attr_inWidth.attr,
    &dev_attr_outWidth.attr,
    &dev_attr_inHeight.attr,
    &dev_attr_outHeight.attr,
    &dev_attr_maxDisBlocks.attr,
    &dev_attr_disV4H2scan.attr,
    &dev_attr_biCubicOff.attr,
    &dev_attr_cisBsize.attr,
    &dev_attr_limitCbuff.attr,
    &dev_attr_disLumaTiles.attr,
    &dev_attr_disChromaTiles.attr,
    &dev_attr_disBlkSize.attr,
    &dev_attr_disBlkChromaSize.attr,
    &dev_attr_blankLuma.attr,
    &dev_attr_blankChroma.attr,
    &dev_attr_validDisBlks.attr,
    &dev_attr_inBitDepth.attr,
    &dev_attr_outBitDepth.attr,
    &dev_attr_inFmt.attr,
    &dev_attr_outFmt.attr,
    NULL,
};

static const struct attribute_group dewarp_attr_group = {
    .attrs = dewarp_attrs,
};

int dewarp_create_sysfs_dev_files(struct dewarp_subdev_dev *dw_dev)
{
    int ret;

    dewarp_class = class_create(THIS_MODULE, "dewarp");
    if (IS_ERR(dewarp_class)) {
        pr_err("Failed to create dewarp class\n");
        return PTR_ERR(dewarp_class);
    }

    dewarp_class_dev = device_create(dewarp_class, NULL,
                                     MKDEV(0, 0), dw_dev, "lut_cfg");
    if (IS_ERR(dewarp_class_dev)) {
        pr_err("Failed to create dewarp class device\n");
        class_destroy(dewarp_class);
        return PTR_ERR(dewarp_class_dev);
    }

    ret = sysfs_create_group(&dewarp_class_dev->kobj, &dewarp_attr_group);
    if (ret) {
        pr_err("Failed to create sysfs group\n");
        device_destroy(dewarp_class, MKDEV(0, 0));
        class_destroy(dewarp_class);
        return ret;
    }

    return 0;
}

void dewarp_remove_sysfs_dev_files(struct dewarp_subdev_dev *dw_dev)
{
    sysfs_remove_group(&dewarp_class_dev->kobj, &dewarp_attr_group);
    device_destroy(dewarp_class, MKDEV(0, 0));
    class_destroy(dewarp_class);
}
