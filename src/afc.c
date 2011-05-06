/*
 * afc.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/afc.h>

#include "afc.h"
#include "device.h"
#include "lockdown.h"

afc_t* afc_open(device_t* device) {
	int err = 0;
	afc_t* afc = NULL;

	afc = (afc_t*) malloc(sizeof(afc_t));
	if(afc == NULL) {
		return NULL;
	}
	memset(afc, '\0', sizeof(afc_t));

	return afc;
}


int afc_send_file(afc_t* afc, const char* local, const char* remote) {
	//TODO: Implement Me
	return -1;
}


int afc_close(afc_t* afc) {
	//TODO: Implement Me
	return -1;
}

void afc_free(afc_t* afc) {
	if(afc) {
		free(afc);
	}
}
