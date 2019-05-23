/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <device.h>
#include <soc.h>
#include "ipm_mhu.h"

#define DEV_CFG(dev) \
	((const struct ipm_mhu_device_config * const)(dev)->config->config_info)
#define DEV_DATA(dev) \
	((struct ipm_mhu_data *)(dev)->driver_data)
#define IPM_MHU_REGS(dev) \
	((volatile struct ipm_mhu_reg_map_t *)(DEV_CFG(dev))->base)

static enum ipm_mhu_cpu_id_t ipm_mhu_get_cpu_id(const struct device *d)
{
	volatile u32_t *p_mhu_dev_base;
	volatile u32_t *p_cpu_id;

	p_mhu_dev_base = (volatile u32_t *)IPM_MHU_REGS(d);

	p_cpu_id = (volatile u32_t *)(((u32_t)p_mhu_dev_base &
						SSE_200_DEVICE_BASE_REG_MSK) +
						SSE_200_CPU_ID_UNIT_OFFSET);

	return (enum ipm_mhu_cpu_id_t)*p_cpu_id;
}

static u32_t ipm_mhu_get_status(const struct device *d,
						enum ipm_mhu_cpu_id_t cpu_id,
						u32_t *status)
{
	struct ipm_mhu_reg_map_t *p_mhu_dev;

	if (status == NULL) {
		return IPM_MHU_ERR_INVALID_ARG;
	}

	p_mhu_dev = (struct ipm_mhu_reg_map_t *)IPM_MHU_REGS(d);

	switch (cpu_id) {
	case IPM_MHU_CPU1:
		*status = p_mhu_dev->cpu1intr_stat;
		break;
	case IPM_MHU_CPU0:
	default:
		*status = p_mhu_dev->cpu0intr_stat;
		break;
	}

	return IPM_MHU_ERR_NONE;
}

static int ipm_mhu_send(struct device *d, int wait, u32_t cpu_id,
			  const void *data, int size)
{
	ARG_UNUSED(wait);
	ARG_UNUSED(data);
	const u32_t set_val = 0x01;

	struct ipm_mhu_reg_map_t *p_mhu_dev;

	if (cpu_id >= IPM_MHU_CPU_MAX) {
		return -EINVAL;
	}

	if (size > IPM_MHU_MAX_DATA_SIZE) {
		return -EMSGSIZE;
	}

	p_mhu_dev = (struct ipm_mhu_reg_map_t *)IPM_MHU_REGS(d);

	switch (cpu_id) {
	case IPM_MHU_CPU1:
		p_mhu_dev->cpu1intr_set = set_val;
		break;
	case IPM_MHU_CPU0:
	default:
		p_mhu_dev->cpu0intr_set = set_val;
		break;
	}

	return 0;
}

static void ipm_mhu_clear_val(const struct device *d,
						enum ipm_mhu_cpu_id_t cpu_id,
						u32_t clear_val)
{
	struct ipm_mhu_reg_map_t *p_mhu_dev;

	p_mhu_dev = (struct ipm_mhu_reg_map_t *)IPM_MHU_REGS(d);

	switch (cpu_id) {
	case IPM_MHU_CPU1:
		p_mhu_dev->cpu1intr_clr = clear_val;
		break;
	case IPM_MHU_CPU0:
	default:
		p_mhu_dev->cpu0intr_clr = clear_val;
		break;
	}
}

static u32_t ipm_mhu_max_id_val_get(struct device *d)
{
	ARG_UNUSED(d);

	return IPM_MHU_MAX_ID_VAL;
}

static int ipm_mhu_init(struct device *d)
{
	const struct ipm_mhu_device_config *config = DEV_CFG(d);

	config->irq_config_func(d);

	return 0;
}

static void ipm_mhu_isr(void *arg)
{
	struct device *d = arg;
	struct ipm_mhu_data *driver_data = DEV_DATA(d);
	enum ipm_mhu_cpu_id_t cpu_id;
	u32_t ipm_mhu_status;

	cpu_id = ipm_mhu_get_cpu_id(d);

	ipm_mhu_get_status(d, cpu_id, &ipm_mhu_status);
	ipm_mhu_clear_val(d, cpu_id, ipm_mhu_status);

	if (driver_data->callback) {
		driver_data->callback(driver_data->callback_ctx, cpu_id,
					&ipm_mhu_status);
	}
}

static int ipm_mhu_set_enabled(struct device *d, int enable)
{
	ARG_UNUSED(d);
	ARG_UNUSED(enable);
	return 0;
}

static int ipm_mhu_max_data_size_get(struct device *d)
{
	ARG_UNUSED(d);

	return IPM_MHU_MAX_DATA_SIZE;
}

static void ipm_mhu_register_cb(struct device *d,
						ipm_callback_t cb,
						void *context)
{
	struct ipm_mhu_data *driver_data = DEV_DATA(d);

	driver_data->callback = cb;
	driver_data->callback_ctx = context;
}

static const struct ipm_driver_api ipm_mhu_driver_api = {
	.send = ipm_mhu_send,
	.register_callback = ipm_mhu_register_cb,
	.max_data_size_get = ipm_mhu_max_data_size_get,
	.max_id_val_get = ipm_mhu_max_id_val_get,
	.set_enabled = ipm_mhu_set_enabled,
};

static void ipm_mhu_irq_config_func_0(struct device *d);

static const struct ipm_mhu_device_config ipm_mhu_cfg_0 = {
	.base = (u8_t *)DT_ARM_MHU_0_BASE_ADDRESS,
	.irq_config_func = ipm_mhu_irq_config_func_0,
};

static struct ipm_mhu_data ipm_mhu_data_0 = {
	.callback = NULL,
	.callback_ctx = NULL,
};

DEVICE_AND_API_INIT(mhu_0,
			DT_ARM_MHU_0_LABEL,
			&ipm_mhu_init,
			&ipm_mhu_data_0,
			&ipm_mhu_cfg_0, PRE_KERNEL_1,
			CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
			&ipm_mhu_driver_api);

static void ipm_mhu_irq_config_func_0(struct device *d)
{
	ARG_UNUSED(d);
	IRQ_CONNECT(DT_ARM_MHU_0_IRQ_0,
			DT_ARM_MHU_0_IRQ_0,
			ipm_mhu_isr,
			DEVICE_GET(mhu_0),
			0);
	irq_enable(DT_ARM_MHU_0_IRQ_0);
}

static void ipm_mhu_irq_config_func_1(struct device *d);

static const struct ipm_mhu_device_config ipm_mhu_cfg_1 = {
	.base = (u8_t *)DT_ARM_MHU_1_BASE_ADDRESS,
	.irq_config_func = ipm_mhu_irq_config_func_1,
};

static struct ipm_mhu_data ipm_mhu_data_1 = {
	.callback = NULL,
	.callback_ctx = NULL,
};

DEVICE_AND_API_INIT(mhu_1,
			DT_ARM_MHU_1_LABEL,
			&ipm_mhu_init,
			&ipm_mhu_data_1,
			&ipm_mhu_cfg_1, PRE_KERNEL_1,
			CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
			&ipm_mhu_driver_api);

static void ipm_mhu_irq_config_func_1(struct device *d)
{
	ARG_UNUSED(d);
	IRQ_CONNECT(DT_ARM_MHU_1_IRQ_0,
			DT_ARM_MHU_1_IRQ_0_PRIORITY,
			ipm_mhu_isr,
			DEVICE_GET(mhu_1),
			0);
	irq_enable(DT_ARM_MHU_1_IRQ_0);
}
