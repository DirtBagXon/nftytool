/*
 *  nftytool - a utility for the Infinity USB card programmer
 * 
 *  Copyright (C) 2003  Erik Thyrén
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _INFINITY_H_
#define _INFINITY_H_

#include <usb.h>

#define INFINITY_VENDOR    0x104f
#define INFINITY_PRODUCT   0x0001

#define USB_ENDPOINT_XFER_INT 0x03


#define PIC_CMD_CFG_MEM_LOAD        0x00

#define PIC_CMD_PROG_MEM_LOAD       0x02
#define PIC_CMD_PROG_MEM_READ       0x04

#define PIC_CMD_DATA_MEM_LOAD       0x03
#define PIC_CMD_DATA_MEM_READ       0x05

#define PIC_CMD_PROG_MEM_ERASE      0x09
#define PIC_CMD_DATA_MEM_ERASE      0x0b

#define PIC_CMD_INCR                0x06
#define PIC_CMD_ERASE_WRITE         0x08
#define PIC_CMD_WRITE               0x18

#define PIC_CMD_CP_DISABLE_SEQ_1    0x01
#define PIC_CMD_CP_DISABLE_SEQ_2    0x07

#define PHOENIX_POL_SMARTMOUSE      0x00
#define PHOENIX_POL_PHOENIX         0x01

#define PHOENIX_FREQ_NONE           0x00
#define PHOENIX_FREQ_600            0x01
#define PHOENIX_FREQ_358            0x02
#define PHOENIX_FREQ_368            0x03


struct usb_infinity {
	struct usb_device *dev;
	struct usb_dev_handle *handle;
	struct usb_endpoint_descriptor *ep_in, *ep_out;
};

struct usb_device *infinity_get_device(int device);
struct usb_endpoint_descriptor *infinity_get_ep_descriptor(struct usb_infinity *inf, char direction);

int infinity_read_data(struct usb_infinity *inf, unsigned char *buf, int len);
int infinity_write_data(struct usb_infinity *inf, unsigned char *buf, int len);

/* general functions */

#define LED_PROGRAMMING    0xe6
#define LED_IDLE           0xf0

int infinity_set_led1(struct usb_infinity *inf, unsigned char brightness);

int infinity_get_status(struct usb_infinity *inf, unsigned char *status);

int infinity_get_firmware_version(struct usb_infinity *inf, unsigned char *resp);
int infinity_get_product_name(struct usb_infinity *inf, unsigned char *resp);

/* sleep functions */

int infinity_set_delay_register(struct usb_infinity *inf, unsigned char delay);
int infinity_millisleep(struct usb_infinity *inf, unsigned char delay);
int infinity_microsleep(struct usb_infinity *inf, unsigned char delay);

/* phoenix functions */

int infinity_phoenix_enable(struct usb_infinity *inf, unsigned char polarity, unsigned char frequency);
int infinity_phoenix_disable(struct usb_infinity *inf);
	
/* PIC functions */

int infinity_pic_power_on(struct usb_infinity *inf);
int infinity_pic_power_off(struct usb_infinity *inf);

int infinity_pic_reset(struct usb_infinity *inf);

int infinity_pic_cmd(struct usb_infinity *inf, unsigned char cmd);
int infinity_pic_cmd_load(struct usb_infinity *inf, unsigned char cmd, unsigned char *data);
int infinity_pic_cmd_read(struct usb_infinity *inf, unsigned char cmd, unsigned char *resp);

int infinity_pic_prog_mem_inc(struct usb_infinity *inf);
int infinity_pic_prog_mem_incm(struct usb_infinity *inf, unsigned char len);

int infinity_pic_prog_mem_write(struct usb_infinity *inf, unsigned char *data);
int infinity_pic_prog_mem_read(struct usb_infinity *inf, unsigned char *data);
int infinity_pic_prog_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len);

int infinity_pic_data_mem_write(struct usb_infinity *inf, unsigned char *data);
int infinity_pic_data_mem_read(struct usb_infinity *inf, unsigned char *data);

/* AVR functions */

int infinity_avr_power_on(struct usb_infinity *inf);
int infinity_avr_power_off(struct usb_infinity *inf);

int infinity_avr_cycle_clock(struct usb_infinity *inf);

int infinity_avr_reset(struct usb_infinity *inf);

int infinity_avr_prog_mem_reset(struct usb_infinity *inf);
int infinity_avr_prog_mem_inc(struct usb_infinity *inf);
int infinity_avr_prog_mem_incm(struct usb_infinity *inf, unsigned char len);

int infinity_avr_prog_mem_read(struct usb_infinity *inf, unsigned char *data);
int infinity_avr_prog_mem_write(struct usb_infinity *inf, unsigned char *data);
int infinity_avr_prog_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len);
int infinity_avr_prog_mem_writem(struct usb_infinity *inf, unsigned char *data, unsigned int len);

int infinity_avr_data_mem_write(struct usb_infinity *inf, unsigned char data);
int infinity_avr_data_mem_read(struct usb_infinity *inf, unsigned char *data);
int infinity_avr_data_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len);

/* eeprom functions */

int infinity_eeprom_power_on(struct usb_infinity *inf);
int infinity_eeprom_power_off(struct usb_infinity *inf);

int infinity_eeprom_write_byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char data);
int infinity_eeprom_write_byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char data);
int infinity_eeprom_write_8byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data);
int infinity_eeprom_write_16byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data);
int infinity_eeprom_write_32byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data);
int infinity_eeprom_write_64byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data);

int infinity_eeprom_read_byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data);
int infinity_eeprom_read_byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data);

int infinity_eeprom_read_group_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data, unsigned char len);
int infinity_eeprom_read_group_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data, unsigned char len);


#endif
