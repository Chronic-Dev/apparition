/*
 * lockdown.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef LOCKDOWN_H_
#define LOCKDOWN_H_

#include <libimobiledevice/lockdown.h>

struct device;
typedef struct lockdown_t {
	struct device_t* device;
	lockdownd_client_t client;
} lockdown_t;

lockdown_t* lockdown_init(struct device_t* device);
int lockdown_open(lockdown_t* lockdown);
int lockdown_start_service(lockdown_t* lockdown, const char* service, int* port);
int lockdown_stop_service(lockdown_t* lockdown, const char* service);
int lockdown_close(lockdown_t* lockdown);
void lockdown_free(lockdown_t* lockdown);

#endif /* LOCKDOWN_H_ */
