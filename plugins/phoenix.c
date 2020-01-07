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
#include <unistd.h>
#include <getopt.h>
#include "infinity.h"
#include "plugin.h"


#define ACTION_DISABLE 0
#define ACTION_ENABLE  1
#define ACTION_STATUS  2


static void show_usage(void) {
	
	printf("Options: \n");
	printf("         -f, --freq <frequency>  Select frequency for player:\n");
	printf("                                 none, 3.58, 3.68, 6.00\n");
	printf("         -p, --pol <polarity>    Polarity on reset signal:\n");
	printf("                                 phoenix, smartmouse\n");
	printf("         -s, --status            Displays current phoenix status\n");
	printf("         -e, --enable            Enables phoenix mode\n");
	printf("         -d, --disable           Disables phoenix mode\n");
	printf("         -h, --help              Prints this message\n");
}

static void pol_parse(const char *pol, unsigned char *val) {

	if (!strcmp("phoenix", pol))
		*val = PHOENIX_POL_PHOENIX;
	else if (!strcmp("smartmouse", pol))
		*val = PHOENIX_POL_SMARTMOUSE;
	else 
		*val = -1;	
}

static void freq_parse(const char *freq, unsigned char *val) {
	
	if (!strcmp("none", freq))
		*val = PHOENIX_FREQ_NONE;
	else if (!strcmp("3.58", freq))
		*val = PHOENIX_FREQ_358;
	else if (!strcmp("3.68", freq))
		*val = PHOENIX_FREQ_368;
	else if (!strcmp("6.00", freq))
		*val = PHOENIX_FREQ_600;
	else 
		*val = -1;		
}

static int status(struct usb_infinity *inf) {
	
	unsigned char s = 0;
	
	infinity_get_status(inf, &s);

	printf("Status:    %s\n", (s & 0x20) ? "enabled" : "disabled");
	printf("Polarity:  %s\n", (s & 0x10) ? "phoenix" : "smartmouse");
	printf("Frequency: ");
	
	switch ((s >> 2) & 0x03) {
	case PHOENIX_FREQ_NONE:
		printf("none\n");
		break;
		
	case PHOENIX_FREQ_600:
		printf("6.00 MHz\n");
		break;

	case PHOENIX_FREQ_358:
		printf("3.58 MHz\n");
		break;
		
	case PHOENIX_FREQ_368:
		printf("3.68 MHz\n");
		break;
	}
	
	return 0;
}

int get_info(struct plugin *plugin) {
	
	plugin->id = strdup("phoenix");
	plugin->name = strdup("Infinity USB phoenix");
			
	return 0;
}

int run(struct usb_infinity *inf, struct arg *arg, int argc, char **argv) {
	
	int i;
	int c;
	int action;
	int opt_index;
	unsigned char stats, freq, pol;;
	struct option opt_options[] = {
		{ "enable",  0, NULL, 'e' },
		{ "disable", 0, NULL, 'd' },
		{ "pol",     1, NULL, 'p' },
		{ "freq",    1, NULL, 'f' },
		{ "status",  0, NULL, 's' },
		{ "help",   0, NULL, 'h' },
		{ NULL,      0, NULL, 0   }
	};
	
	action = -1;
	stats = pol = freq = 0xff;
	optind = opterr = optopt = 0;
		
	while ( (c = getopt_long(argc, argv, "hedp:f:s", opt_options, &opt_index)) != -1 ) {
		
		switch (c) {
		case '?':
			show_usage();
			return -1;
			
		case 'h':
			show_usage();
			return 0;
			
		case 'e':
			if (action != -1) {
				fprintf(stderr, "Only one of enable/disable/status must be chosen\n");
				return -1;
			}
			action = ACTION_ENABLE;
			break;

		case 'd':
			if (action != -1) {
				fprintf(stderr, "Only one of enable/disable/status must be chosen\n");
				return -1;
			}
			action = ACTION_DISABLE;
			break;
			
		case 'p':
			pol_parse(optarg, &pol);
			break;

		case 'f':
			freq_parse(optarg, &freq);
			break;

		case 's':
			if (action != -1) {
				fprintf(stderr, "Only one of enable/disable/status must be chosen\n");
				return -1;
			}
			action = ACTION_STATUS;
			break;

		}
	}
		
	switch (action) {
	
	case ACTION_ENABLE:
		
		infinity_get_status(inf, &stats);
		
		// no pol 
		if (pol == 0xff)
			pol = (stats & 0x10) >> 4;
		
		// no freq 
		if (freq == 0xff)
			freq = (stats & 0x0c) >> 2;

		infinity_phoenix_enable(inf, pol, freq);
		break;
		
	case ACTION_DISABLE:
		
		infinity_phoenix_disable(inf);
		break;

	case ACTION_STATUS:

		status(inf);
		break;
	
	default:
		fprintf(stderr, "Phoenix: You must specify an action to perform\n");
		break;
	}
	
	return 0;
}


