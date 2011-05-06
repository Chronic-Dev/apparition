/*
 * lockdown.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef LOCKDOWN_H_
#define LOCKDOWN_H_

typedef struct lockdown_t {
	idevice_t* device;
	lockdownd_client_t client;
} lockdown_t;

int lockdown_start_service(const char* service, int* port);
int lockdown_stop_service(const char* service);

#endif /* LOCKDOWN_H_ */
