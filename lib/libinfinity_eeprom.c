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


// turns power supply on
int infinity_eeprom_power_on(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x37;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// turns power supply off
int infinity_eeprom_power_off(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x38;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// writes a byte to address given in 8 bit format
int infinity_eeprom_write_byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char data) {

  unsigned char buf[4];
  
  buf[0] = 0x39;
  buf[1] = ctrl;
  buf[2] = addr;
  buf[3] = data;
  
  infinity_write_data(inf, buf, 4);
      
  return 0;
}

// writes a byte to address given in 16 bit format
int infinity_eeprom_write_byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char data) {

  unsigned char buf[8];
  
  buf[0] = 0x3a;
  buf[1] = ctrl;
  buf[2] = addr[0];
  buf[3] = addr[1];
  buf[4] = data;
  
  infinity_write_data(inf, buf, 5);
      
  return 0;
}

// writes 8 bytes to address given in 8 bit format
int infinity_eeprom_write_8byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data) {

  unsigned char buf[12];
  
  buf[0] = 0x3b;
  buf[1] = ctrl;
  buf[2] = addr;
  memcpy(&buf[3], data, 8);
  
  infinity_write_data(inf, buf, 11);
      
  return 0;
}

// writes 16 bytes to address given in 8 bit format
int infinity_eeprom_write_16byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data) {

  unsigned char buf[20];
  
  buf[0] = 0x3c;
  buf[1] = ctrl;
  buf[2] = addr;
  memcpy(&buf[3], data, 16);
  
  infinity_write_data(inf, buf, 19);
      
  return 0;
}

// writes 32 bytes to address given in 16 bit format
int infinity_eeprom_write_32byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data) {

  unsigned char buf[36];
  
  buf[0] = 0x3d;
  buf[1] = ctrl;
  buf[2] = addr[0];
  buf[3] = addr[1];
  memcpy(&buf[4], data, 32);
  
  infinity_write_data(inf, buf, 36);
      
  return 0;
}

// writes 64 bytes to address given in 16 bit format
int infinity_eeprom_write_64byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data) {

  unsigned char buf[68];
  
  buf[0] = 0x3e;
  buf[1] = ctrl;
  buf[2] = addr[0];
  buf[3] = addr[1];
  memcpy(&buf[4], data, 64);
  
  infinity_write_data(inf, buf, 68);
      
  return 0;
}

// reads a byte from address given in 8 bit format
// increments program counter by 1
int infinity_eeprom_read_byte_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x3f;
  buf[1] = ctrl;
  buf[2] = addr;
  
  infinity_write_data(inf, buf, 3);
  
  infinity_read_data(inf, data, 1);
      
  return 0;
}

// reads a byte from address given in 16 bit format
// increments program counter by 1
int infinity_eeprom_read_byte_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x40;
  buf[1] = ctrl;
  buf[2] = addr[0];
  buf[3] = addr[1];
  
  infinity_write_data(inf, buf, 4);
  
  infinity_read_data(inf, data, 1);
   
  return 0;
}

// reads a group of (8 * len) bytes from address given in 8 bit format
// increments program counter by len
int infinity_eeprom_read_group_addr8(struct usb_infinity *inf, unsigned char ctrl, unsigned char addr, unsigned char *data, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x41;
  buf[1] = ctrl;
  buf[2] = addr;
  buf[3] = len;
  
  infinity_write_data(inf, buf, 4);
  
  infinity_read_data(inf, data, len * 8);
   
  return 0;
}

// reads a group of (8 * len) bytes from address given in 16 bit format
// increments program counter by len
int infinity_eeprom_read_group_addr16(struct usb_infinity *inf, unsigned char ctrl, unsigned char *addr, unsigned char *data, unsigned char len) {

  unsigned char buf[8];
  
  buf[0] = 0x42;
  buf[1] = ctrl;
  buf[2] = addr[0];
  buf[3] = addr[1];
  buf[4] = len;
  
  infinity_write_data(inf, buf, 5);
  
  infinity_read_data(inf, data, len * 8);
   
  return 0;
}

