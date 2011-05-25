/*
 * device.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <libimobiledevice/libimobiledevice.h>

struct lockdown_t;
typedef struct device_t {
	idevice_t client;
	unsigned char* uuid;
	unsigned char* ecid;
	struct lockdown_t* lockdown;
} device_t;

device_t* device_open(const char* uuid);
void device_free(device_t* device);
void device_enable_debug();
int device_connect(device_t* device, unsigned short port, idevice_connection_t* connection);

#endif /* DEVICE_H_ */
