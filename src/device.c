/*
 * device.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

/* This is the application Controller.
 *  This file should contain functions to control the device
 *  through lockdownd services. This file acts as a proxy to
 *  the afc, mb2, and nos services and should handle messages.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

device_t* device_create(char* uuid) {
	int err = 0;
	device_t* device = NULL;
	
	device = (device_t*) malloc(sizeof(device_t));
	if(device == NULL) {
		return NULL;
	}
	memset(device, '\0', sizeof(device_t));
	
	return device;
}

void device_free() {
	//TODO: Implement Me
}
