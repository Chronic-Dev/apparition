/*
 * crashreportmover.h
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#ifndef CRASHREPORTMOVER_H_
#define CRASHREPORTMOVER_H_

#include <libimobiledevice/libimobiledevice.h>

#include "lockdown.h"

typedef struct crashreportmover_t {
	unsigned short port;
	idevice_connection_t connection;
} crashreportmover_t;

crashreportmover_t* crashreportermover_open(lockdown_t* lockdown);
crashreportmover_t* crashreportermover_create(lockdown_t* lockdown);
int crashreportermover_close(crashreportmover_t* mover);
void crashreportermover_free(crashreportmover_t* mover);

#endif /* CRASHREPORTMOVER_H_ */
