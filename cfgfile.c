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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cfgfile.h"

static unsigned char config_delay_register = 10; // 10 msec is recommended
static unsigned char config_led_idle_brightness = 1;
static char config_plugins_path[256];


// copies the plugins directory path to path
// there must be enough space allocated in path
int config_get_plugins_path(char *path) {
  
  if (config_plugins_path[0])
    strcpy(path, config_plugins_path);
  else
    return -1;
	
  return 0;
}

// copies the value of the delay register
int config_get_delay_register(unsigned char *delay_register) {

  *delay_register = config_delay_register;
  
  return 0;
}

int config_get_led_idle_brightness(unsigned char *led_idle_brightness) {

  *led_idle_brightness = config_led_idle_brightness;
  
  return 0;
}


int config_read_file(const char *path) {

	FILE *fp;
	int v;
	char buf[256];
	char obj[256], val[256];
	struct stat s;
	
	
	fp = fopen(path, "r");
	if (!fp) {
		fprintf(stderr, "Couldn't read config file '%s'\n", path);
		return -1;
	}
	
	while (fgets(buf, 255, fp)) {
		
		memset(val, '\0', 255);
		sscanf(buf, "%s %255[^\n]", obj, val);
		
		if (!strcmp(obj, "plugins_path")) {
			
			if (!stat(val, &s)) {
				if (S_ISDIR(s.st_mode)) {
					strncpy(config_plugins_path, val, 255);
				} else {
					printf("%s isn't a directory\n", val);
					fclose(fp);
					return -1;
				}
			} else {
				printf("Couldn't open directory %s\n", val);
				fclose(fp);
				return -1;
			}
			
		} else if (!strcmp(obj, "led_idle_brightness")) {
			
			v = atoi(val);
			
			if (v > 0 && v <= 16)
				config_led_idle_brightness = (0xf0 | v) - 1;
			else {
				printf("Uknown LED parameter %s\n", val);
				fclose(fp);
				return -1;
			}

		} else {
			
			fprintf(stderr, "Unknown option %s\n", obj);
			fclose(fp);
			return -1;
		}
		
	}
	
	fclose(fp);
  
	return 0;
}
