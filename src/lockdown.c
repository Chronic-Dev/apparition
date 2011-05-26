/*
 * lockdown.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/lockdown.h>

#include "device.h"
#include "lockdown.h"

lockdown_t* lockdown_init(device_t* device) {
	lockdown_t* lockdown = (lockdown_t*) malloc(sizeof(lockdown_t));
	if (lockdown == NULL) {
		return NULL;
	}
	memset(lockdown, '\0', sizeof(lockdown_t));
	lockdown->device = device;
	return lockdown;
}

lockdown_t* lockdown_open(device_t* device) {
	if(device->lockdown == NULL) {
	 device->lockdown = lockdown_init(device);
		if(device->lockdown == NULL) {
			return NULL;
		}
	}

	lockdownd_client_t lockdownd = NULL;
	if (lockdownd_client_new_with_handshake(device->client, &lockdownd, "apparition") != LOCKDOWN_E_SUCCESS) {
		return NULL;
	}
	device->lockdown->client = lockdownd;
	return device->lockdown;
}

int lockdown_start_service(lockdown_t* lockdown, const char* service, int* port) { //cant figure out hwo to get this working, too many levels of pointers for the port between this and lockdownd_start_service

	uint64_t port_value = 0;
	lockdownd_start_service(lockdown->client, service, &port_value);

	if (port_value) {
		printf("Started %s successfully!\n", service);
		*port = port_value;
		return 0;
	} else {

		printf("%s failed to start!\n", service);
		return -1;
	}

	return -1;
}

int lockdown_stop_service(lockdown_t* lockdown, const char* service) {
	//TODO: Implement Me
	return -1;
}

int lockdown_close(lockdown_t* lockdown) {
	lockdownd_goodbye(lockdown->client);
	lockdown->client = NULL;
	return 0;
}

void lockdown_free(lockdown_t* lockdown) {
	if (lockdown) {
		if (lockdown->client) {
			lockdown_close(lockdown);
		}
		free(lockdown);
	}
}
