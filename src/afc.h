/*
 * afc.h
 * Functions for lockdownd's apple file conduit service
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef AFC_H_
#define AFC_H_

#include <libimobiledevice/afc.h>
#include "device.h"

typedef struct afc_t {
	afc_client_t client;
} afc_t;

afc_t* afc_open(device_t* device);
int afc_send_file(afc_t* afc, const char* local, const char* remote);
int afc_close(afc_t* afc);
void afc_free(afc_t* afc);

#endif /* AFC_H_ */
