/*
 * nos.h
 * Functions for lockdownd's notification service
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef NOS_H_
#define NOS_H_

#include <libimobiledevice/notification_proxy.h>

#include "lockdown.h"

typedef struct nos_t {
	lockdown_t* lockdown;
	np_client_t* client;
} nos_t;

nos_t* nos_open(lockdown_t* lockdown);
int nos_register(nos_t* nos, char* callback, void* whatever);
void nos_free(nos_t* nos);

#endif /* NOS_H_ */
