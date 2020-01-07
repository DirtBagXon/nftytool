/*
 *  nftytool - a utility for the Infinity USB card programmer
 * 
 *  Copyright (C) 2005  Erik Thyrén
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
#include <getopt.h>
#include "infinity.h"
#include "plugin.h"
#include "hexfile.h"
#include "goldcard.h"

int get_info(struct plugin *plugin) {
	
	plugin->id = strdup("gc");
	plugin->name = strdup("Goldcard (PIC16F84A + 24C16)");
	
	return 0;
}

static int show_usage(void) {
	
	printf("Options:\n");
	printf("      -e <file>, --ext-read        Reads external eeprom from card to <file>\n");
	printf("      -f <file>, --ext-write       Writes from <file> to external eeprom\n");
	printf("      -p <file>, --pic-read        Reads from PIC to <file>\n");
	printf("      -q <file>, --pic-write       Writes from <file> to PIC\n");
	printf("      -c, --pic-clear              Clears the PIC\n");
	printf("      -l, --pic-loader             Writes loader to PIC\n");
	printf("      -x, --pic-cfg                Shows the PIC cfg and ID\n");
	printf("      -h, --help                   Displays this message\n");

	return 0;
}

static int pic_disable_cp(struct usb_infinity *inf) {
	
	unsigned char buf[4];
	
	// 16 bits set, start and stop bits are 0
	buf[0] = 0x7f;
	buf[1] = 0xfe; 
	infinity_pic_cmd_load(inf, PIC_CMD_CFG_MEM_LOAD, buf);
	
	// increment til CW
	infinity_pic_prog_mem_incm(inf, 7);
	
	// do the CP disable magic
	infinity_pic_cmd(inf, PIC_CMD_CP_DISABLE_SEQ_1);
	infinity_pic_cmd(inf, PIC_CMD_CP_DISABLE_SEQ_2);
	
	infinity_pic_cmd(inf, PIC_CMD_ERASE_WRITE);
	
	infinity_millisleep(inf, 10);

	infinity_pic_cmd(inf, PIC_CMD_CP_DISABLE_SEQ_1);
	infinity_pic_cmd(inf, PIC_CMD_CP_DISABLE_SEQ_2);
	
	return 0;
}

static int pic_clear(struct usb_infinity *inf, struct arg *arg) {
	
	unsigned char status;
	unsigned char buf[4];
	
	buf[0] = 0x7f;
	buf[1] = 0xfe; // all bits set except for start and stop bit
		
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}
	
	infinity_set_led1(inf, LED_PROGRAMMING);
	
	infinity_pic_power_on(inf);
	
	pic_disable_cp(inf);
	
	// erase prog mem
	infinity_pic_cmd_load(inf, PIC_CMD_PROG_MEM_LOAD, buf);
	infinity_pic_cmd(inf, PIC_CMD_PROG_MEM_ERASE);	
	infinity_pic_cmd(inf, PIC_CMD_ERASE_WRITE);
	infinity_millisleep(inf, 10);

	// erase data mem
	infinity_pic_cmd_load(inf, PIC_CMD_DATA_MEM_LOAD, buf);
	infinity_pic_cmd(inf, PIC_CMD_DATA_MEM_ERASE);
	infinity_pic_cmd(inf, PIC_CMD_ERASE_WRITE);
	infinity_millisleep(inf, 10);
	
	infinity_pic_power_off(inf);
	
	infinity_set_led1(inf, arg->led1);
	
	return 0;
}

// a special case.. could just as well be done by pic_write()
static int pic_loader(struct usb_infinity *inf, struct arg *arg) {
	
	unsigned int i;
	unsigned int size;
	unsigned char status;
	unsigned char cmd_arg[4];
	
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}
	
	memset(cmd_arg, 0, sizeof(cmd_arg));
	
	// we start by erasing the whole card
	pic_clear(inf, arg);
	
	infinity_set_led1(inf, LED_PROGRAMMING);
	        
	infinity_set_delay_register(inf, 10);
	
	infinity_pic_power_on(inf);
	
	for (i = 0, size = sizeof(loader_code) ; i < size ; i += 2)
		infinity_pic_prog_mem_write(inf, &loader_code[i]);
	
	infinity_pic_cmd_load(inf, PIC_CMD_CFG_MEM_LOAD, cmd_arg);
	infinity_pic_prog_mem_incm(inf, 7);
	infinity_pic_prog_mem_write(inf, loader_cfg);
	
	infinity_pic_power_off(inf);
	
	infinity_set_led1(inf, arg->led1);
	    
	return 0;
}

static int ext_read(struct usb_infinity *inf, struct arg *arg, const char *path) {
	
	int i;
	unsigned char status;
	unsigned char addr;
	unsigned char ctrl;
	unsigned int len;
	unsigned char *ptr;
	unsigned char *buf;
	struct hexfile hex;
	struct hexdata hdata;
	
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}
		
	buf = (unsigned char *)malloc(GC_EXT_EEPROM_SIZE);
	if (!buf) {
		fprintf(stderr, "Couldn't allocate mem for data\n");
		return -1;
	}
	
	if (hexfile_create(path, &hex)) {
		free(buf);
		return -1;
	}
	
	ptr = buf;
	len = 0;
	
	infinity_set_led1(inf, LED_PROGRAMMING);
	
	infinity_set_delay_register(inf, 10);
	
	infinity_eeprom_power_on(inf);
	
	// the programmer adds read bit!
	for (ctrl = 0xa0 ; ctrl <= 0xaf ; ctrl += 2) {
		for (addr = 0 ; addr < 8 ; addr++) {
			infinity_eeprom_read_group_addr8(inf, ctrl, addr * 32, ptr, 4);
			
			ptr += 32;
			len += 32;
		}
	}
	
	infinity_eeprom_power_off(inf);

	hdata.len = 16;
	hdata.addr = HEXFILE_ADDRESS_INCR;
	for (i = 0 ; i < len ; i += 16) {
		memcpy(hdata.data, &buf[i], hdata.len);
		hexfile_put(&hex, &hdata);
	}
	
	hexfile_close(&hex);
	
	infinity_set_led1(inf, arg->led1);

	free(buf);
	
	return 0;
}

static int ext_write(struct usb_infinity *inf, struct arg *arg, const char *path) {
	
	unsigned char status;
	unsigned char addr;
	unsigned char ctrl;
	unsigned char *ptr;
	unsigned char *buf;
	unsigned int len;
	struct hexfile hex;
	struct hexdata hdata;
	
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}
	
	if (hexfile_open(path, &hex))
		return -1;
	
	len = 0;
	buf = (unsigned char *)malloc(GC_EXT_EEPROM_SIZE);
	if (!buf) {
		hexfile_close(&hex);
		return -1;
	}

	ptr = buf;
	hdata.addr = HEXFILE_ADDRESS_INCR;
	while (hexfile_get(&hex, &hdata) != -1) {
		if (hdata.len == 0)
			break;
		
		if (len + hdata.len > GC_EXT_EEPROM_SIZE) {
			fprintf(stderr, "File too big to fit inside eeprom!\n");
			hexfile_close(&hex);
			return -1;
		}
		
		memcpy(ptr, hdata.data, hdata.len);
		
		ptr += hdata.len;
		len += hdata.len;
	}
	hexfile_close(&hex);
	
	/*********************
	 * start programming *
	 *********************/
	
	infinity_set_led1(inf, LED_PROGRAMMING);
	
	infinity_set_delay_register(inf, 10);
	
	infinity_eeprom_power_on(inf);
	
	ptr = buf;
	for (ctrl = 0xa0 ; ctrl <= 0xae ; ctrl += 2) {
		for (addr = 0 ; addr < 16 ; addr++) {
			infinity_eeprom_write_16byte_addr8(inf, ctrl, addr * 16, ptr);
			ptr += 16;
		}
	}
	
	infinity_eeprom_power_off(inf);
	infinity_set_led1(inf, arg->led1);
	
	free(buf);

	return 0;
}

static int pic_read(struct usb_infinity *inf, struct arg *arg, const char *path) {

	int i, j;
	unsigned char status;
	unsigned char *ptr;
	unsigned char *buf;
	unsigned char cmd_arg[4];
	struct hexfile hex;
	struct hexdata hdata;
		
	//
	//ROM=0000-07FF, EEPROM=4200-427F, Config: 400E-400F, Ident: 4000-4007
	//    14bit             8bit               14bit             8bit, optional
	//
	//
	
	
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}

	memset(cmd_arg, 0, 4);
	
	buf = (unsigned char *)malloc(GC_ROM_SIZE + GC_EEPROM_SIZE + GC_IDENT_SIZE + GC_CONFIG_SIZE);
	ptr = buf;
	if (!buf) {
		fprintf(stderr, "Couldn't allocate memory\n");
		return -1;
	}
	
	if (hexfile_create(path, &hex)) {
		free(buf);
		return -1;
	}
	
	
	infinity_set_led1(inf, LED_PROGRAMMING);

	infinity_pic_power_on(inf);
      
	/** prog mem **/

	for (i = 0 ; i < 4 ; i++) {
		infinity_pic_prog_mem_readm(inf, ptr, 0xff);
		ptr += (0xff * 2);
	}
	infinity_pic_prog_mem_readm(inf, ptr, 0x04);
	
	ptr = buf;
	hdata.addr = HEXFILE_ADDRESS_INCR;
	hdata.len = 0x10;
	for (i = 0 ; i < 128 ; i++) {
		convertArrayFromPicFormat(ptr, hdata.len);
		
		memcpy(hdata.data, ptr, hdata.len);

		hexfile_put(&hex, &hdata);
		
		ptr += hdata.len;
	}
	
	
	/** I.D. **/
	
	hdata.addr = GC_IDENT_POS;
	hdata.len = GC_IDENT_SIZE; // read whole chunk at once
	
	infinity_pic_cmd_load(inf, PIC_CMD_CFG_MEM_LOAD, cmd_arg);

	infinity_pic_prog_mem_readm(inf, hdata.data, 4);
		
	convertArrayFromPicFormat(hdata.data, hdata.len);
	
	hexfile_put(&hex, &hdata);
	
	
	/** config word **/
	
	hdata.addr = GC_CONFIG_POS;
	hdata.len = GC_CONFIG_SIZE; // read whole chunk at once
	
	infinity_pic_prog_mem_incm(inf, 3);
	
	infinity_pic_prog_mem_read(inf, hdata.data);
	
	convertArrayFromPicFormat(hdata.data, hdata.len);
	
	hexfile_put(&hex, &hdata);
	
	
	/** data mem **/

	infinity_pic_reset(inf);
	
	hdata.addr = GC_EEPROM_POS;
	hdata.len = 0x10;
	for (i = 0 ; i < 8 ; i++) {
		for (j = 0 ; j < hdata.len ; j += 2)
			infinity_pic_data_mem_read(inf, &hdata.data[j]);
		
		convertArrayFromPicFormat(hdata.data, hdata.len);
		
		for (j = 0 ; j < hdata.len ; j += 2)
			hdata.data[j + 1] = 0x00;
		
		hexfile_put(&hex, &hdata);
		
		// stupid to do this _every_ iteration
		// set addr to incremental mode
		hdata.addr = HEXFILE_ADDRESS_INCR;
	}
	
	//infinity_pic_reset(inf);
	
	/*********/

	hexfile_close(&hex);
	
	free(buf);
		
	infinity_pic_power_off(inf);
	
	infinity_set_led1(inf, arg->led1);

	return 0;
}

static int pic_write(struct usb_infinity *inf, struct arg *arg, const char *path) {
	
	int i;
 	int retval;
	unsigned int addr;
	unsigned char status;
	unsigned char cmd_arg[4];
	struct hexfile hex;	
	struct hexdata hdata;
	
	retval = 0;
	memset(cmd_arg, 0, 4);
	
	infinity_get_status(inf, &status);
	if ((status & 1) == 0) {
		fprintf(stderr, "No card present in programmer\n");
		return -1;
	}
	
	if (hexfile_open(path, &hex))
		return -1;
	
	// we start by erasing the whole card
	pic_clear(inf, arg);
	
	infinity_set_led1(inf, LED_PROGRAMMING);
	
	infinity_set_delay_register(inf, 10);
	
	infinity_pic_power_on(inf);

	/** progmem **/
	
	addr = 0;
	for ( ; ; ) {
		
		if (hexfile_get(&hex, &hdata)) {
			fprintf(stderr, "Error in program memory in '%s'\n", path);
			retval = -1;
			goto out;
		}
		
		if (!hdata.len)
			goto out;
		
		//  prog mem segment ends at 0x7ff
		if (hdata.addr >= GC_ROM_SIZE)
			break;
		
		// if there's a gap in the file
		if ( hdata.addr - addr ) {
			
			for (i = (hdata.addr - addr) / 2 ; i > 255 ; i -= 255)
				infinity_pic_prog_mem_incm(inf, 255);
			
			infinity_pic_prog_mem_incm(inf, i);
		}
		
		addr = hdata.addr + hdata.len;
		
		convertArrayToPicFormat(hdata.data, hdata.len);
		
		for (i = 0 ; i < hdata.len ; i += 2) 
			infinity_pic_prog_mem_write(inf, &hdata.data[i]);

	}
	
	/** ID **/
	
	infinity_pic_cmd_load(inf, PIC_CMD_CFG_MEM_LOAD, cmd_arg);
	
	// we do a rewind here to be on the safe side
	if (hexfile_seek(&hex, GC_IDENT_POS, 1)) {
		fprintf(stderr, "Couldn't find ID in '%s'\n", path);
		
		// increment past the ID
		infinity_pic_prog_mem_incm(inf, 4);
	
	} else {
		
		if (hexfile_get(&hex, &hdata)) {
			fprintf(stderr, "Error in program memory in '%s'\n", path);
			retval = -1;
			goto out;
		}
		
		convertArrayToPicFormat(hdata.data, hdata.len);
		
		for (i = 0 ; i < hdata.len ; i += 2)
			infinity_pic_prog_mem_write(inf, &hdata.data[i]);
		
		// if the ID is 'broken'
		infinity_pic_prog_mem_incm(inf, (8 - hdata.len) / 2);
	}

	/* Config word */
	
	if (hexfile_seek(&hex, GC_CONFIG_POS, 1)) {
		
		fprintf(stderr, "Couldn't find config word in '%s'\n", path);

	} else {
		
		if (hexfile_get(&hex, &hdata)) {
			fprintf(stderr, "Error in config word in '%s'\n", path);
			retval = -1;
			goto out;
		}
		
		convertArrayToPicFormat(hdata.data, hdata.len);

		infinity_pic_prog_mem_incm(inf, 3);
		infinity_pic_prog_mem_write(inf, hdata.data);
	}

	/* Data mem */
	
	if (hexfile_seek(&hex, GC_EEPROM_POS, 1)) {
		
		fprintf(stderr, "Couldn't find data memory in '%s'\n", path);
	
	} else {
		
		infinity_pic_reset(inf);
		
		for ( ; ; ) {
			
			if (hexfile_get(&hex, &hdata)) {
				fprintf(stderr, "Error in data memory in '%s'\n", path);
				retval = -1;
				goto out;
			}
						
			if (!hdata.len)
				goto out;
			
			// data mem segment ends at 0x427f
			if (hdata.addr >= GC_EEPROM_POS + GC_EEPROM_SIZE)
				break;
			
			// FIXME - can there be gaps in data mem?
			
			convertArrayToPicFormat(hdata.data, hdata.len);
						
			for (i = 0 ; i < hdata.len ; i += 2)
				infinity_pic_data_mem_write(inf, &hdata.data[i]);
		}
	}
	
	out:	
	
	infinity_pic_power_off(inf);
	
	infinity_set_led1(inf, arg->led1);
		
	hexfile_close(&hex);
	
	return retval;
}

static int pic_config_read(struct usb_infinity *inf, struct arg *arg) {
	
	int i;
	unsigned char cmd_arg[4];
	unsigned char buf[20];
		
	memset(cmd_arg, 0, 4);
	
	infinity_pic_power_on(inf);
	
	infinity_pic_cmd_load(inf, PIC_CMD_CFG_MEM_LOAD, cmd_arg);
	
	infinity_pic_prog_mem_readm(inf, buf, 4);
	
	fprintf(stderr, "ID: ");
	for (i = 0 ; i < 8 ; i += 2)
		fprintf(stderr, "%02x%02x ", buf[i], buf[i+1]);
	fprintf(stderr, "\n");
	

	infinity_pic_prog_mem_incm(inf, 3);
	infinity_pic_prog_mem_readm(inf, buf, 1);
	
	fprintf(stderr, "CW: %02x %02x\n", buf[0], buf[1]);
	
	infinity_pic_power_off(inf);
	
	return 0;
}

int run(struct usb_infinity *inf, struct arg *arg, int argc, char **argv) {
	
	int c;
	int opt_index;
	char *e_read, *e_write, *p_read, *p_write;
	struct option opt_options[] = {
		{ "pic-read",   1, NULL, 'p' },
		{ "pic-write",  1, NULL, 'q' },
		{ "ext-read",   1, NULL, 'e' },
		{ "ext-write",  1, NULL, 'f' },
		{ "help",       0, NULL, 'h' },
		{ "pic-clear",  0, NULL, 'c' },
		{ "pic-cfg",    0, NULL, 'x' },
		{ "pic-loader", 0, NULL, 'l' },
		{ NULL,         0, NULL, 0   }
	};
	
	e_read = e_write = p_read = p_write = NULL;
	optind = opterr = optopt = 0;
		
	while ( (c = getopt_long(argc, argv, "xlhp:q:e:f:c", opt_options, &opt_index)) != -1 ) {
		
		switch (c) {
		case '?':
			show_usage();
			return -1;
			
		case 'h':
			show_usage();
			return 0;
			
		case 'p':
			p_read = strdup(optarg);
			break;

		case 'q':
			p_write = strdup(optarg);
			break;
			
		case 'e':
			e_read = strdup(optarg);
			break;

		case 'f':
			e_write = strdup(optarg);
			break;

		case 'l':
			pic_loader(inf, arg);
			break;
			
		case 'c':
			pic_clear(inf, arg);
			break;

		case 'x':
			pic_config_read(inf, arg);
			break;

		}
	}
	
	
	if (p_read)
		pic_read(inf, arg, p_read);
	
	if (e_read)
		ext_read(inf, arg, e_read);
	
	if (e_write)
		ext_write(inf, arg, e_write);

	if (p_write)
		pic_write(inf, arg, p_write);

	
	
	return 0;
}
