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
#include "nos.h"
#include "device.h"
#include "lockdown.h"

typedef struct afc_t {
	lockdown_t* lockdown;
	nos_t* notifier;
	afc_client_t client;
} afc_t;

afc_t* afc_open(nos_t* notifier);
int afc_send_file(afc_t* afc, const char* local, const char* remote);
int afc_close(afc_t* afc);
void afc_free(afc_t* afc);

#endif /* AFC_H_ */
