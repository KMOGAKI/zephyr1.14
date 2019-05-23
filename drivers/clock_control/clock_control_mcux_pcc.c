/*
 * Copyright (c) 2019 Vestas Wind Systems A/S
 *
 * Based on clock_control_rv32m1_pcc.c, which is:
 * Copyright (c) 2018 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <soc.h>
#include <clock_control.h>
#include <fsl_clock.h>

#define LOG_LEVEL CONFIG_CLOCK_CONTROL_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(clock_control_mcux_pcc);

struct mcux_pcc_config {
	u32_t base_address;
};

#define DEV_CFG(dev)  ((struct mcux_pcc_config *)(dev->config->config_info))
#define DEV_BASE(dev) (DEV_CFG(dev)->base_address)
#ifndef MAKE_PCC_REGADDR
#define MAKE_PCC_REGADDR(base, offset) ((base) + (offset))
#endif

static inline clock_ip_name_t clock_ip(struct device *dev,
				       clock_control_subsys_t sub_system)
{
	u32_t offset = POINTER_TO_UINT(sub_system);

	return MAKE_PCC_REGADDR(DEV_BASE(dev), offset);
}

static int mcux_pcc_on(struct device *dev, clock_control_subsys_t sub_system)
{
	CLOCK_EnableClock(clock_ip(dev, sub_system));
	return 0;
}

static int mcux_pcc_off(struct device *dev, clock_control_subsys_t sub_system)
{
	CLOCK_DisableClock(clock_ip(dev, sub_system));
	return 0;
}

static int mcux_pcc_get_rate(struct device *dev,
			       clock_control_subsys_t sub_system,
			       u32_t *rate)
{
	*rate = CLOCK_GetIpFreq(clock_ip(dev, sub_system));
	return 0;
}

static int mcux_pcc_init(struct device *dev)
{
	return 0;
}

static const struct clock_control_driver_api mcux_pcc_api = {
	.on = mcux_pcc_on,
	.off = mcux_pcc_off,
	.get_rate = mcux_pcc_get_rate,
};

#if defined(DT_MCUX_PCC_0_NAME)
static const struct mcux_pcc_config mcux_pcc0_config = {
	.base_address = DT_MCUX_PCC_0_BASE_ADDRESS
};

DEVICE_AND_API_INIT(mcux_pcc0, DT_MCUX_PCC_0_NAME,
		    &mcux_pcc_init,
		    NULL, &mcux_pcc0_config,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_OBJECTS,
		    &mcux_pcc_api);
#endif
