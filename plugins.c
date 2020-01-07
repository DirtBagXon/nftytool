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
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include "plugins.h"

// loads a plugin from path
// into plugin
int plugins_load(const char *path, struct plugin *plugin) {
	
	void *h;
	int (*get_info)(struct plugin *plugin);
	
	h = dlopen(path, RTLD_NOW);
	if (!h) {
		fprintf(stderr, "Couldn't open plugin '%s'\n", path);
		return -1;
	}
	
	get_info = dlsym(h, "get_info");
	if (!get_info) {
		fprintf(stderr, "Couldn't find required function get_info() in '%s'\n", path);
		dlclose(h);
		return -1;
	}
	
	if (get_info(plugin)) {
		fprintf(stderr, "Couldn't get information from plugin\n");
		dlclose(h);
		return -1;
	}
	
	dlclose(h);
	
	return 0;
}

// comparing function to get
// a file that ends with .so
int plugins_cmp_so(const struct dirent *ent) {
	
	char *ptr;
	
	ptr = strrchr(ent->d_name, '.');
	if (ptr) {
		if (!strcmp(".so", ptr))
			return 1;
	}
	
	return 0;
}

// loads all the plugins (*.so) that exist
// in directory given by path and stores
// them in plugins
int plugins_load_all(const char *path, struct plugin ***plugins) {
	
	int i;
	int entries;
	char buf[256];
	struct dirent **list;
	
	entries = scandir(path, &list, plugins_cmp_so, alphasort);
	if (entries < 0)
		return -1;
	else {
		*plugins = (struct plugin **)malloc(sizeof (struct plugin *) * (entries + 1));
		(*plugins)[entries] = NULL;
		
		for (i = 0 ; i < entries ; i++) {
			(*plugins)[i] = (struct plugin *)malloc(sizeof (struct plugin));
			snprintf(buf, 255, "%s/%s", path, list[i]->d_name) < 0 ? abort() : (void)0;
			(*plugins)[i]->path = strdup(buf);
			
			if (plugins_load((*plugins)[i]->path, (*plugins)[i])) {
				while (i--) {
					
					free((*plugins)[i]->id);
					free((*plugins)[i]->name);
					free((*plugins)[i]->path);
					
					free((*plugins)[i]);
					free(list[i]);
				}
				free(*plugins);
				free(list);
				return -1;
			}
			
			
			free(list[i]);
		}
		
		free(list);
	}
	
	return 0;
}
