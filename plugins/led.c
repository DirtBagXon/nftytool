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

int get_info(struct plugin *plugin) {
	
	plugin->id = strdup("led");
	plugin->name = strdup("infinity USB LED");
		
	return 0;
}

static void show_usage(void) {
	
	printf("Options: \n");
	printf("         -l, --led1 <brightness>  Sets led brightness\n");
	printf("         -h, --help               Prints this message\n");
}

int run(struct usb_infinity *inf, struct arg *arg, int argc, char **argv) {
	
	int i;
	int c;
	unsigned char b;
	int opt_index;
	struct option opt_options[] = {
		{ "led1",  1, NULL, 'l' },
		{ "help",  0, NULL, 'h' },
		{ NULL,    0, NULL, 0   }
	};
	
	b = 0;
	optind = opterr = optopt = 0;
		
	while ( (c = getopt_long(argc, argv, "hl:", opt_options, &opt_index)) != -1 ) {
		
		switch (c) {
		case '?':
			show_usage();
			return -1;
			
		case 'h':
			show_usage();
			return 0;
			
		case 'l':
			b = atoi(optarg);
			if (b > 0 && b <= 16)
				b--;
			infinity_set_led1(inf, 0xf0 | b);
			
			break;
		}
	}
	
	return 0;
}


