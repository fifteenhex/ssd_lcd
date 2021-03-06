/*
 * cmd_dfu.c -- dfu command
 *
 * Copyright (C) 2012 Samsung Electronics
 * authors: Andrzej Pietrasiewicz <andrzej.p@samsung.com>
 *	    Lukasz Majewski <l.majewski@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dfu.h>
#include <g_dnl.h>
#include <usb.h>

static int do_dfu(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bool dfu_reset = false;

	if (argc < 4)
		return CMD_RET_USAGE;

	char *usb_controller = argv[1];
	char *interface = argv[2];
	char *devstring = argv[3];

	int ret, i = 0;

	ret = dfu_init_env_entities(interface, devstring);
	if (ret)
		goto done;

	ret = CMD_RET_SUCCESS;
	if (argc > 4 && strcmp(argv[4], "list") == 0) {
		dfu_show_entities();
		goto done;
	}

	int controller_index = simple_strtoul(usb_controller, NULL, 0);
	(void)controller_index;
	g_dnl_clear_detach();
	g_dnl_register("usb_dnl_dfu");
	while (1) {
		if (g_dnl_detach()) {
			/*
			 * Check if USB bus reset is performed after detach,
			 * which indicates that -R switch has been passed to
			 * dfu-util. In this case reboot the device
			 */
			if (dfu_usb_get_reset()) {
				dfu_reset = true;
				goto exit;
			}

			/*
			 * This extra number of usb_gadget_handle_interrupts()
			 * calls is necessary to assure correct transmission
			 * completion with dfu-util
			 */
			if (++i == 10000)
				goto exit;
		}

		if (ctrlc())
			goto exit;

		usb_gadget_handle_interrupts();
	}
exit:
	g_dnl_unregister();
done:
	dfu_free_entities();

	if (dfu_reset)
		run_command("reset", 0);

	g_dnl_clear_detach();

	return ret;
}

U_BOOT_CMD(dfu, CONFIG_SYS_MAXARGS, 1, do_dfu,
	"Device Firmware Upgrade",
	"<USB_controller> <interface> <dev> [list]\n"
	"  - device firmware upgrade via <USB_controller>\n"
	"    on device <dev>, attached to interface\n"
	"    <interface>\n"
	"    [list] - list available alt settings\n"
);

int update_dfu(void)
{
	int dfu_enable = 0;

	dfu_enable = getenv_ulong("enter_dfu_mode", 10, 0);
	if (dfu_enable)
	{
		run_command("set enter_dfu_mode 0", 0);
		run_command("saveenv", 0);

#if defined (CONFIG_DFU_NAND) && defined (CONFIG_DFU_NAND)
		run_command("dfu 0 nand 0", 0);
#elif defined (CONFIG_DFU_SF)
		run_command("dfu 0 sf 0:0", 0);
#elif defined (CONFIG_DFU_RAM)
		run_command("dfu 0 ram 0", 0);
#else
		printf("unknown dfu update type\n");
#endif
	}
	return 0;
}
