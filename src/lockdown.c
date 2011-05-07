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


lockdown_t* lockdown_open(device_t* device) {
	int err = 0;
	lockdown_t* lockdown = NULL;

	lockdown = (lockdown_t*) malloc(sizeof(lockdown_t));
	if(lockdown == NULL) {
		return NULL;
	}
	memset(lockdown, '\0', sizeof(lockdown_t));

	lockdownd_client_t lockdownd = NULL;
	if (lockdownd_client_new_with_handshake(device->client, &lockdownd, "apparition") != LOCKDOWN_E_SUCCESS) {
		device_free(device);
		return NULL;
	}

	lockdown->client = lockdownd;

	return lockdown;
}

int lockdown_start_service(lockdown_t* lockdown, const char* service, int* port) {
	//TODO: Implement Me
	return -1;
}

int lockdown_stop_service(lockdown_t* lockdown, const char* service) {
	//TODO: Implement Me
	return -1;
}


int lockdown_close(lockdown_t* lockdown) {
	//TODO: Implement Me
	return -1;
}

void lockdown_free(lockdown_t* lockdown) {
	if(lockdown) {
		if(lockdown->client) {
			lockdownd_goodbye(lockdown->client);
			lockdown->client = NULL;
		}
		free(lockdown);
	}
}
