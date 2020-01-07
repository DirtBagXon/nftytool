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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hexfile.h"

char hex2dec(char hex) {
   
	int num;
	
	switch ( hex ) {
	case 'A' ... 'F':
		num = 10 + (hex - 'A');
		break;
	case 'a' ... 'f':
		num = 10 + (hex - 'a');
		break;
	default:
		num = hex - '0';
	}
	
	return num;   
}

static inline void frompic(unsigned char *pair) {
	
	unsigned int tal;
	
	tal = pair[0] << 8 | pair[1];

	tal >>= 1;

	pair[0] = (tal & 0xff);
	pair[1] = (tal & 0xff00) >> 8;
}

static inline void topic(unsigned char *pair) {
	
	unsigned int tal;
		
	tal = pair[1] << 8 | pair[0];

	tal <<= 1;

	pair[0] = (tal & 0xff00) >> 8;
	pair[1] = (tal & 0xff);
}

void convertArrayToPicFormat(unsigned char *buf, int len) {
	
	int i;
	
	for ( i = 0 ; i < len ; i += 2 ) {
		topic(&buf[i]);
	}
}

void convertArrayFromPicFormat(unsigned char *buf, int len) {
	
	int i;
	
	for ( i = 0 ; i < len ; i += 2 ) {
		frompic(&buf[i]);
	}
}

static void addExtension(struct hexfile *hex) {
   
	unsigned char checksum;
	
	hex->address = 0;
	hex->extension++;
	
	checksum = 02 + 04 + (hex->extension & 0xff00) + (hex->extension & 0x00ff);
	checksum = (~checksum) + 1;
	
	fprintf(hex->fp, ":020000%02X%02X%02X%02X\r\n", HEXFILE_TYPE_EXT, (hex->extension & 0xff00), (hex->extension & 0x00ff), checksum);
}

int hexfile_seek(struct hexfile *hex, unsigned int address, int whence) {
	
	fpos_t pos, org;
	char buf[256];
	char tmp[20];
	
	fgetpos(hex->fp, &org);
	pos = org;
	
	if (whence)
		rewind(hex->fp);

	snprintf(tmp, 10, "%04X", address);

	while (fgets(buf, 255, hex->fp)) {
		
		if (!strncmp(&buf[3], tmp, 4)) {
			fsetpos(hex->fp, &pos);
			return 0;
		}
		
		fgetpos(hex->fp, &pos);
	}
	
	fsetpos(hex->fp, &org);
	
	return 1;
}
 
int hexfile_open(const char *path, struct hexfile *hex) {
   
	
	if ( !hex ) {
		fprintf(stderr, "Invalid hexfile reference\n");
		return -1;
	}
	
	hex->fp = fopen(path, "r");
	if ( !hex->fp ) {
		fprintf(stderr, "Couldn't open file '%s' for reading\n", path);
		return -1;
	}
	
	hex->path = strdup(path);
	if ( !hex->path ) {
		fprintf(stderr, "Out of memory\n");
		fclose(hex->fp);
		return -1;
	}
   
	hex->address = 0;
	hex->extension = 0;
	hex->direction = HEXFILE_DIRECTION_READ;
	
	return 0;
}

int hexfile_create(const char *path, struct hexfile *hex) {
   
	
	if ( !hex ) {
		fprintf(stderr, "Invalid hexfile reference\n");
		return -1;
	}
	
	hex->fp = fopen(path, "w");
	if ( !hex->fp ) {
		fprintf(stderr, "Couldn't open file '%s' for writing\n", path);
		return -1;
	}
   
	hex->path = strdup(path); 
	if ( !hex->path ) {
		fprintf(stderr, "Out of memory\n");
		fclose(hex->fp);
		return -1;
	}
   
	hex->address = 0;
	hex->extension = 0;
	hex->direction = HEXFILE_DIRECTION_WRITE;
	
	return 0;
}

int hexfile_close(struct hexfile *hex) {
   
	if ( hex->direction == HEXFILE_DIRECTION_WRITE ) {
		fprintf(hex->fp, ":00000001FF\r\n");  // this is constant for EOF
	}
   
	fclose(hex->fp);
	free(hex->path);
	
	return 0;
}

int hexfile_get(struct hexfile *hex, struct hexdata *hdata) {
   
	int i;
	unsigned char data[256];
	unsigned char rec_len;
	unsigned char rec_addr[2];
	unsigned char rec_type;
	unsigned char rec_checksum;
	unsigned char checksum;

	fgets((char *)data, 255, hex->fp);
	
	if ( data[0] != ':' )
		return -1;
	
	if ( !isxdigit(data[1]) || !isxdigit(data[2]) )
		return -1;
	else 
		checksum = rec_len = (hex2dec(data[1]) << 4) | hex2dec(data[2]);
		
	if ( !isxdigit(data[3]) || !isxdigit(data[4]) )
		return -1;
	else
		checksum += rec_addr[0] = (hex2dec(data[3]) << 4) | hex2dec(data[4]);
	
	if ( !isxdigit(data[5]) || !isxdigit(data[6]) )
		return -1;
	else
		checksum += rec_addr[1] = (hex2dec(data[5]) << 4) | hex2dec(data[6]);
	
	if ( !isxdigit(data[7]) || !isxdigit(data[8]) )
		return -1;
	else 
		checksum += rec_type = (hex2dec(data[7]) << 4) | hex2dec(data[8]);
	
	//do some checking on the type here
	if (rec_type == HEXFILE_TYPE_EXT)
		return hexfile_get(hex, hdata);
		
	for ( i = 0 ; i < (rec_len << 1) ; i += 2 ) {
		if ( !isxdigit(data[9 + i]) || !isxdigit(data[10 + i]) )
			return -1;
		else 
			checksum += hdata->data[i >> 1] = (hex2dec(data[9 + i]) << 4) | hex2dec(data[10 + i]);
	}
	
	if ( !isxdigit(data[9 + i]) || !isxdigit(data[10 + i]) )
		return -1;
	else
		rec_checksum = (hex2dec(data[9 + i]) << 4) | hex2dec(data[10 + i]);

	hex->address += rec_len;
	
	checksum = (~checksum) + 1;
	if ( checksum != rec_checksum )
		return -1;
	
	hdata->len = rec_len;
	hdata->addr = (unsigned int)(rec_addr[0] << 8 | rec_addr[1]);
	
	return 0;
}

int hexfile_put(struct hexfile *hex, struct hexdata *hdata) {
   
	int i;
	unsigned char checksum;
	
	if (hdata->addr != HEXFILE_ADDRESS_INCR)
		hex->address = hdata->addr;
	
	if ( hex->address >= 0xffff ) {
		addExtension(hex);
	}
	
	checksum = hdata->len;
	checksum += ((hex->address & 0xff00) >> 8);
	checksum += (hex->address & 0x00ff);
	fprintf(hex->fp, ":%02X%04X%02X", hdata->len, hex->address, 00); //type is always 00
	for ( i = 0 ; i < hdata->len ; i++ ) {
		fprintf(hex->fp, "%02X", hdata->data[i]);
		checksum += hdata->data[i];
	}
	checksum = (~checksum) + 1;
	fprintf(hex->fp, "%02X\r\n", checksum);
	
	hex->address += hdata->len;

	return 0;
}
