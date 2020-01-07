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

int infinity_pic_cmd(struct usb_infinity *inf, unsigned char cmd) {
	
	unsigned char buf[4];
	
	buf[0] = 0x0a;
	buf[1] = cmd;
	
	infinity_write_data(inf, buf, 2);
	
	return 0;
}

int infinity_pic_cmd_load(struct usb_infinity *inf, unsigned char cmd, unsigned char *data) {

	unsigned char buf[4];
	
	buf[0] = 0x0b;
	buf[1] = cmd;
	buf[2] = data[0];
	buf[3] = data[1];
	
	infinity_write_data(inf, buf, 4);

	return 0;
}

int infinity_pic_cmd_read(struct usb_infinity *inf, unsigned char cmd, unsigned char *resp) {
	
	unsigned char buf[4];
	
	buf[0] = 0x0c;
	buf[1] = cmd;
	
	infinity_write_data(inf, buf, 2);
	
	infinity_read_data(inf, resp, 2);

	return 0;
}

int infinity_pic_power_on(struct usb_infinity *inf) {
  
  unsigned char buf[4];
  
  buf[0] = 0x0d;
  
  infinity_write_data(inf, buf, 1);
  
  return 0;
}

int infinity_pic_power_off(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x0e;
  
  infinity_write_data(inf, buf, 1);
  
  return 0;
}

int infinity_pic_reset(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x16;
  
  infinity_write_data(inf, buf, 1);
  
  return 0;
}

// increments the program counter by 1
int infinity_pic_prog_mem_inc(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x0f;
  
  infinity_write_data(inf, buf, 1);
  
  return 0;
}

// increments the program counter by len
int infinity_pic_prog_mem_incm(struct usb_infinity *inf, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x10;
  buf[1] = len;
  
  infinity_write_data(inf, buf, 2);
  
  return 0;
}

// writes a word to prog mem
// increases program counter with 1
int infinity_pic_prog_mem_write(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x11;
  buf[1] = data[0];
  buf[2] = data[1];
  
  infinity_write_data(inf, buf, 3);
  
  return 0;
}

// reads a word from prog mem
// prog counter is increased by 1
int infinity_pic_prog_mem_read(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x12;
  
  infinity_write_data(inf, buf, 1);
  
  infinity_read_data(inf, data, 2);
    
  return 0;
}

// reads len words from prog mem to data
// prog counter is increased by len
int infinity_pic_prog_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x13;
  buf[1] = len;
  
  infinity_write_data(inf, buf, 2);
  
  infinity_read_data(inf, data, len * 2);
    
  return 0;
}

// writes a word to pic data mem
// prog counter is increased by 1
int infinity_pic_data_mem_write(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x14;
  buf[1] = data[0];
  buf[2] = data[1];
  
  infinity_write_data(inf, buf, 3);
    
  return 0;
}

// reads a word from pic data mem to buffer data
// prog counter is increased by 1
int infinity_pic_data_mem_read(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x15;
  
  infinity_write_data(inf, buf, 1);
  
  infinity_read_data(inf, data, 2);
      
  return 0;
}

