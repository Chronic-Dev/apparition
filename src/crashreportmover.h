/*
 * crashreportmover.h
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#ifndef CRASHREPORTMOVER_H_
#define CRASHREPORTMOVER_H_

#include <libimobiledevice/libimobiledevice.h>

typedef struct crashreportmover {
	unsigned short port;
	idevice_connection_t connection;
} crashreportmover;

crashreportermover_t* crashreportermover_open(lockdown_t* lockdown);
crashreportermover_t* crashreportermover_create(lockdown_t* lockdown);
int crashreportermover_close(crashreportermover_t* mover);
void crashreportermover_free(crashreportermover_t* mover);

#endif /* CRASHREPORTMOVER_H_ */
