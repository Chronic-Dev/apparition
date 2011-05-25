/*
 * lockdown.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef LOCKDOWN_H_
#define LOCKDOWN_H_

#include <libimobiledevice/lockdown.h>

struct afc_t;
struct nos_t;
struct mb2_t;
struct device_t;
struct crashreporter_t;
typedef struct lockdown_t {
	struct afc_t* afc;
	struct nos_t* nos;
	struct mb2_t* mb2;
	struct device_t* device;
	struct crashreporter_t* crashreporter;
	lockdownd_client_t client;
} lockdown_t;

lockdown_t* lockdown_init(struct device_t* device);
lockdown_t* lockdown_open(struct device_t* device);
int lockdown_start_service(lockdown_t* lockdown, const char* service, int* port);
int lockdown_stop_service(lockdown_t* lockdown, const char* service);
int lockdown_close(lockdown_t* lockdown);
void lockdown_free(lockdown_t* lockdown);

#endif /* LOCKDOWN_H_ */
