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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <usb.h>
#include <dlfcn.h>
#include "infinity.h"
#include "cfgfile.h"
#include "plugins.h"


void exit_usage(const char *progname, int exitval);
void exit_arg(const char *progname, const char *arg, int exitval);

void exit_usage(const char *progname, int exitval) {
   
	printf("Usage: %s [options]\n", progname);
	printf("Options: \n");
	printf("         -p <plugin>     Select plugin to work with\n");
	printf("         -l              List all available plugins\n");
	printf("         -h              Show this help message\n");
	printf("         -d <device>     Number of programmer to work with if more than one is connected to the computer\n");
	
	exit(exitval);
}

void exit_arg(const char *progname, const char *arg, int exitval) {

	if (arg)
		printf("Invalid argument: '%s'\n", arg);
	
	printf("Try `%s -h' for more information.\n", progname);

	exit(exitval);
}

void usb_start(struct usb_infinity *inf, int devnum) {
	
	usb_init();
	usb_find_busses();
	usb_find_devices();
	
	inf->dev = infinity_get_device(devnum);
	if (!inf->dev) {
		fprintf(stderr, "Couldn't find infinity device\n");
		exit(-1);
	}
	
	inf->handle = usb_open(inf->dev); 
	if (!inf->handle) {
		fprintf(stderr, "Couldn't open infinity device\n");
		exit(-1);
	}

	usb_claim_interface(inf->handle, 0);
	
	inf->ep_out = infinity_get_ep_descriptor(inf, 0);
	inf->ep_in = infinity_get_ep_descriptor(inf, 1);	

}

void usb_stop(struct usb_infinity *inf) {

	usb_release_interface(inf->handle, 0);
	
	usb_reset(inf->handle);
	
	usb_close(inf->handle);
}

int main(int argc, char **argv) {
	
	int i, j, c;
	char *pname, *fname;
	char cfile[256];
	char name[20];
	struct usb_infinity inf;
	struct plugin **plugins;
	struct arg arg;
	int devnum = 0;
	
	void *h;
	int (*func)(struct usb_infinity *, struct arg *, int, char **);
	
	pname = NULL;
	fname = NULL;
	
	if (argc < 2) {
		exit_usage(argv[0], -1);
	}

	if (config_read_file("nftytool.conf"))
		return -1;
	
	config_get_plugins_path(cfile);
	config_get_led_idle_brightness(&(arg.led1));
	
	if (plugins_load_all(cfile, &plugins))
		return -1;
	
	while ( (c = getopt(argc, argv, "lp:d:h")) != -1 ) {
		
		switch (c) {
		case '?':
			exit_arg(argv[0], NULL, -1);
			break;
			
		case 'h':
			exit_usage(argv[0], 0);
			break;
			
		case 'd':
			devnum = atoi(optarg);
			if (devnum < 1) {
				fprintf(stderr, "Error in device number argument\n");
				return -1;
			}
			devnum--;
			break;
			
		case 'l':
			printf("Available plugins:\n");
			for (i = 0 ; plugins[i] ; i++)
				printf("\t%s - %s\n", plugins[i]->id, plugins[i]->name);
			return 0;

		case 'p':
			pname = strdup(optarg);
			break;
						
		}
	}
		

	if (!pname) 
		exit_arg(argv[0], NULL, -1);
	
		
	for (i = 0 ; plugins[i] ; i++) {
		if (!strcmp(pname, plugins[i]->id))
			break;
	}
	
	if (!plugins[i]) {
		fprintf(stderr, "You must specify type of plugin.\n");
		fprintf(stderr, "Available plugins:\n");
		for (i = 0 ; plugins[i] ; i++)
			fprintf(stderr, "\t%s - %s\n", plugins[i]->id, plugins[i]->name);
		
		return -1;
	}

	// should almost never fail
	h = dlopen(plugins[i]->path, RTLD_NOW);
	if (!h) {
		fprintf(stderr, "Couldn't open plugin '%s'\n", plugins[i]->path);
		return -1;
	}
	
	func = dlsym(h, "run");
	if (!func) {
		fprintf(stderr, "Couldn't find function run() in module '%s'\n", plugins[i]->id);
		return -1;
	}

	
   	usb_start(&inf, devnum);
	
	/** display what programmer we're working with */

	infinity_get_product_name(&inf, name);
	fprintf(stdout, "%s ", name);
	infinity_get_firmware_version(&inf, name);
	fprintf(stdout, "%s\n", name);
	
	for (i = 0 ; i < argc ; i++)
		if (!strcmp("--", argv[i]))
			break;
	
	if (func(&inf, &arg, argc - i, &argv[i]))
		fprintf(stderr, "Error while executing module\n");
	
	dlclose(h);

	usb_stop(&inf);
		
	return 0;
}

