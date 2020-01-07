/*
 *  nftytool - a utility for the Infinity USB Card Reader
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

#ifndef _HEXFILE_H_
#define _HEXFILE_H_

#define HEXFILE_DATA_SIZE          0x10

#define HEXFILE_DIRECTION_READ     0x01
#define HEXFILE_DIRECTION_WRITE    0x02

#define HEXFILE_ADDRESS_INCR       0x00

#define HEXFILE_TYPE_EXT           0x04

struct hexfile { 
	char *path;
	FILE *fp;
	char direction;
	unsigned int address;
	unsigned int extension;
};

struct hexdata {
	unsigned char data[HEXFILE_DATA_SIZE];
	unsigned int len;
	unsigned int addr;
};

int hexfile_create(const char *path, struct hexfile *hex);
int hexfile_open(const char *path, struct hexfile *hex);
int hexfile_close(struct hexfile *hex);

int hexfile_put(struct hexfile *hex, struct hexdata *hdata);
int hexfile_get(struct hexfile *hex, struct hexdata *hdata);

int hexfile_seek(struct hexfile *hex, unsigned int address, int whence);

void convertArrayFromPicFormat(unsigned char *buf, int len);
void convertArrayToPicFormat(unsigned char *buf, int len);

#endif
