/*
* usbtty.h- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/


#ifndef __USB_TTY_H__
#define __USB_TTY_H__


#include "usbdcore.h"
#include "usbdcore_omap1510.h"


#define NUM_CONFIGS    1
#define NUM_INTERFACES 1
#define NUM_ENDPOINTS  3

#define EP0_MAX_PACKET_SIZE 64

#define CONFIG_USBD_CONFIGURATION_STR "TTY via USB"
#define CONFIG_USBD_INTERFACE_STR     "Simple Serial Data Interface - Bulk Mode"


#define CONFIG_USBD_SERIAL_OUT_ENDPOINT 2
#define CONFIG_USBD_SERIAL_OUT_PKTSIZE	64
#define CONFIG_USBD_SERIAL_IN_ENDPOINT	1
#define CONFIG_USBD_SERIAL_IN_PKTSIZE	64
#define CONFIG_USBD_SERIAL_INT_ENDPOINT 5
#define CONFIG_USBD_SERIAL_INT_PKTSIZE	16


#define USBTTY_DEVICE_CLASS	COMMUNICATIONS_DEVICE_CLASS
#define USBTTY_DEVICE_SUBCLASS	COMMUNICATIONS_NO_SUBCLASS
#define USBTTY_DEVICE_PROTOCOL	COMMUNICATIONS_NO_PROTOCOL

#define USBTTY_INTERFACE_CLASS	   0xFF /* Vendor Specific */
#define USBTTY_INTERFACE_SUBCLASS  0x02
#define USBTTY_INTERFACE_PROTOCOL  0x01

#define USBTTY_BCD_DEVICE 0x0
#define USBTTY_MAXPOWER	  0x0

#define STR_MANUFACTURER 1
#define STR_PRODUCT	 2
#define STR_SERIAL	 3
#define STR_CONFIG	 4
#define STR_INTERFACE	 5

#endif
