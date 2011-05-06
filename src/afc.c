/*
 * afc.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>

#include "afc.h"
#include "device.h"

afc_t* afc_open(device_t* device) {
	//TODO: Implement Me
	return NULL;
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
	//TODO: Implement Me
}
