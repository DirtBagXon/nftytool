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
int infinity_avr_power_on(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x21;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// turns power supply off
int infinity_avr_power_off(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x22;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// makes one single pulse on SCK
int infinity_avr_cycle_clock(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x23;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// sets reset high for 100 usec and the turns it low
int infinity_avr_reset(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x24;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// resets the internal program counter
// FIXME - change the name to avoid confusion
int infinity_avr_prog_mem_reset(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x25;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// increments the internal program counter by 1
int infinity_avr_prog_mem_inc(struct usb_infinity *inf) {

  unsigned char buf[4];
  
  buf[0] = 0x26;
  
  infinity_write_data(inf, buf, 1);
      
  return 0;
}

// increments the internal program counter by len
int infinity_avr_prog_mem_incm(struct usb_infinity *inf, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x27;
  buf[1] = len;
  
  infinity_write_data(inf, buf, 2);
      
  return 0;
}

// writes a word from data to prog mem
// increments the internal program counter by 1
int infinity_avr_prog_mem_write(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x28;
  buf[1] = data[0];
  buf[2] = data[1];
  
  infinity_write_data(inf, buf, 3);
      
  return 0;
}

// reads a word from prog mem to data
// increments the internal program counter by 1
int infinity_avr_prog_mem_read(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x29;
  
  infinity_write_data(inf, buf, 1);
  
  infinity_read_data(inf, data, 2);
      
  return 0;
}

// reads len words from prog mem to data
// increments the internal program counter by len
int infinity_avr_prog_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x2a;
  buf[1] = len;
  
  infinity_write_data(inf, buf, 2);
  
  infinity_read_data(inf, data, len * 2);
      
  return 0;
}

// writes len words from data to prog mem
// increments the internal program counter by len
int infinity_avr_prog_mem_writem(struct usb_infinity *inf, unsigned char *data, unsigned int len) {

  unsigned char *buf;
  
  buf = (unsigned char *)malloc(len * 2) + 1;
  if (!buf) {
    fprintf(stderr, "Couldn't allocate mem for buffer\n");
	return -1;
  }
  
  buf[0] = 0x2e;
  memcpy(buf + 1, data, len * 2);
  
  infinity_write_data(inf, buf, len * 2 + 1);
	
  free(buf);
	
  return 0;
}

// writes data to data mem
// increments the internal program counter by 1
int infinity_avr_data_mem_write(struct usb_infinity *inf, unsigned char data) {

  unsigned char buf[4];
  
  buf[0] = 0x2b;
  buf[1] = data;
  
  infinity_write_data(inf, buf, 2);
      
  return 0;
}

// reads a byte from data mem to data
// increments the internal program counter by 1
int infinity_avr_data_mem_read(struct usb_infinity *inf, unsigned char *data) {

  unsigned char buf[4];
  
  buf[0] = 0x2c;
  
  infinity_write_data(inf, buf, 1);
  
  infinity_read_data(inf, data, 1);
     
  return 0;
}

// reads len bytes from data mem to buffer data
// increments the internal program counter by len
int infinity_avr_data_mem_readm(struct usb_infinity *inf, unsigned char *data, unsigned char len) {

  unsigned char buf[4];
  
  buf[0] = 0x2d;
  buf[1] = len;
  
  infinity_write_data(inf, buf, 2);
  
  infinity_read_data(inf, data, len);
     
  return 0;
}
