// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2022, Microchip
 */

#include <drivers/gpio.h>
#include <libfdt.h>
#include <stdio.h>
#include <tee_api_defines.h>
#include <tee_api_defines_extensions.h>
#include <tee_api_types.h>
#include <util.h>

struct gpio *gpio_dt_alloc_pin(struct dt_driver_phandle_args *a,
			       TEE_Result *res)
{
	struct gpio *gpio = NULL;

	if (a->args_count != 2) {
		*res = TEE_ERROR_BAD_PARAMETERS;
		return NULL;
	}

	gpio = calloc(1, sizeof(struct gpio));
	if (!gpio) {
		*res = TEE_ERROR_OUT_OF_MEMORY;
		return NULL;
	}

	gpio->pin = a->args[0];
	gpio->dt_flags = a->args[1];

	*res = TEE_SUCCESS;
	return gpio;
}

static char *gpio_get_dt_prop_name(const char *gpio_name)
{
	char *prop_name = NULL;
	int max_len = strlen(gpio_name) + strlen("-gpios") + 1;

	prop_name = calloc(1, max_len);
	if (!prop_name)
		return NULL;

	snprintf(prop_name, max_len, "%s-gpios", gpio_name);

	return prop_name;
}

TEE_Result gpio_dt_get_by_index(const void *fdt, int nodeoffset,
				unsigned int index, const char *gpio_name,
				struct gpio **gpio)
{
	TEE_Result res = TEE_ERROR_GENERIC;
	char *prop_name = NULL;

	prop_name = gpio_get_dt_prop_name(gpio_name);
	if (!prop_name)
		return TEE_ERROR_OUT_OF_MEMORY;

	*gpio = dt_driver_device_from_node_idx_prop(prop_name, fdt, nodeoffset,
						    index, DT_DRIVER_GPIO,
						    &res);
	free(prop_name);

	return res;
}
