/*
 * device.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef DEVICE_H_
#define DEVICE_H_

typedef struct device_t {

} device_t;

device_t* device_open(const char* uuid);
void device_free();

#endif /* DEVICE_H_ */
