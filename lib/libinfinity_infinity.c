/*
 *  nftytool - a utility for the Infinity USB card programmer
 * 
 *  Copyright (C) 2003 Erik Thyrén
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>
#include "infinity.h"

// writes data to the programmer
int infinity_write_data(struct usb_infinity *inf, unsigned char *buf, int len) {
	
	int num;
	num = usb_interrupt_write(inf->handle, inf->ep_out->bEndpointAddress, (const char*)buf, len, 800);
	
	if (num < 0)
		fprintf(stderr, "Error in usb_interrupt_write()\n");
	return 0;
}

// reads data from the programmer
int infinity_read_data(struct usb_infinity *inf, unsigned char *buf, int len) {
	
	int num;
	num = usb_interrupt_read(inf->handle, inf->ep_in->bEndpointAddress, (char *)buf, len, 800);

	if (num < 0)
		fprintf(stderr, "Error in usb_interrupt_read()\n");

	return 0;
}

// scans the usb bus for the infinity programmer
struct usb_device *infinity_get_device(int device) {
   
	struct usb_device *dev;
	struct usb_bus *bus;
	int devnum = device;
	
	for ( bus = usb_get_busses() ; bus ; bus = bus->next ) {
		for ( dev = bus->devices ; dev ; dev = dev->next ) {
			if (dev->descriptor.idVendor == INFINITY_VENDOR && dev->descriptor.idProduct == INFINITY_PRODUCT) {
				if (!devnum)
					return dev;
				else
					devnum--;
			}
		}
	}
   
	return NULL;
}

// gets the given endpoint descriptor
struct usb_endpoint_descriptor *infinity_get_ep_descriptor(struct usb_infinity *inf, char direction) {
   
	int i;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *ep;
	
	interface = &inf->dev->config->interface->altsetting[0];
	
	for ( i = 0 ; i < interface->bNumEndpoints ; ++i ) {
		ep = &interface->endpoint[i];
		
		if ( direction ) {
			if ( (ep->bEndpointAddress & USB_ENDPOINT_DIR_MASK) &&
			     (ep->bEndpointAddress & USB_ENDPOINT_XFER_INT) ) {
				/* int in endpoint */
				return ep;
			} 
		} else {
			if ( !(ep->bEndpointAddress & USB_ENDPOINT_DIR_MASK) &&
			     (ep->bEndpointAddress & USB_ENDPOINT_XFER_INT) ) {
				/* int out endpoint */
				return ep;
			}
		}
	}
	
	return NULL;
}

// controls the led on the programmer
int infinity_set_led1(struct usb_infinity *inf, unsigned char brightness) {
	
	unsigned char buf[4];
	
	buf[0] = 0x04;
	buf[1] = brightness;
   
	infinity_write_data(inf, buf, 2);
	
	return 0;
}

// gets the status register of the programmer
int infinity_get_status(struct usb_infinity *inf, unsigned char *status) {
   
	unsigned char buf[4];
	
	buf[0] = 0x03;

	infinity_write_data(inf, buf, 1);

	infinity_read_data(inf, status, 1);

	return 0;
}

// gets the programmers firmware version
int infinity_get_firmware_version(struct usb_infinity *inf, unsigned char *resp) {
   
	unsigned char buf[8];
	
	buf[0] = 0x01;
	buf[4] = '\0';
	
	infinity_write_data(inf, buf, 1);
	
	infinity_read_data(inf, buf, 4);

	strcpy((char *)resp, (char *)buf);
	
	return 0;
}

// gets the programmers product name
int infinity_get_product_name(struct usb_infinity *inf, unsigned char *resp) {

	unsigned char buf[20];
	
	buf[0] = 0x02;
	buf[16] = '\0';
	
	infinity_write_data(inf, buf, 1);
	
	infinity_read_data(inf, buf, 16);

	strcpy((char *)resp, (char *)buf);
	
	return 0;
}

// sets the register which holds the delay time betweeen writes and reads
int infinity_set_delay_register(struct usb_infinity *inf, unsigned char delay) {

	unsigned char buf[4];
	
	buf[0] = 0x07;
	buf[1] = delay;
  
	infinity_write_data(inf, buf, 2);
	
	return 0;
}

// sleeps delay milliseconds
int infinity_millisleep(struct usb_infinity *inf, unsigned char delay) {

	unsigned char buf[4];
	
	buf[0] = 0x06;
	buf[1] = delay;
	
	infinity_write_data(inf, buf, 2);
	
	return 0;
}

// sleeps delay microseconds
int infinity_microsleep(struct usb_infinity *inf, unsigned char delay) {

	unsigned char buf[4];
	
	buf[0] = 0x05;
	buf[1] = delay;
	
	infinity_write_data(inf, buf, 2);
	
	return 0;
}


int infinity_phoenix_enable(struct usb_infinity *inf, unsigned char polarity, unsigned char frequency) {
	
	unsigned char buf[4];
	
	buf[0] = 0x49;
	buf[1] = (polarity << 2) | frequency;
	
	infinity_write_data(inf, buf, 2);
		
	return 0;
}

int infinity_phoenix_disable(struct usb_infinity *inf) {
	
	unsigned char buf[4];
	
	buf[0] = 0x4a;
	
	infinity_write_data(inf, buf, 1);
	
	return 0;
}

