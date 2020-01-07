#ifndef _CFGFILE_H_
#define _CFGFILE_H_

int config_get_plugins_path(char *path);
int config_get_delay_register(unsigned char *delay_register);
int config_get_led_idle_brightness(unsigned char *led_idle_brightness);
int config_read_file(const char *path);
	

#endif
