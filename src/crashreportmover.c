/*
 * crashreportmover.c
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#include "crashreportmover.h"

crashreportermover_t* crashreportermover_open(lockdown_t* lockdown) {
	int err = 0;
	unsigned short port = 0;
	crashreportmover_t* mover = NULL;

	err = lockdown_start_service(lockdown, "com.apple.crashreportmover", &port);
	if(err < 0 ) {
		return NULL;
	}

	err = device_connect(lockdown->device, crashreporter->port, crashreporter->connection);
	if(err < 0) {
		return NULL;
	}
	idevice_disconnect(connection);
	return mover;
}

crashreportermover_t* crashreportermover_create(lockdown_t* lockdown) {
	crashreportmover_t* mover == NULL;
	if(lockdown == NULL) {
		printf("Unable to create crashreportermover object\n");
		return NULL;
	}

	mover = (crashreportmover_t*) malloc(sizeof(crashreportmover_t));
	if(mover == NULL) {
		return NULL;
	}

	return mover;
}

int crashreportermover_close(crashreportermover_t* mover) {
	return -1;
}

void crashreportermover_free(crashreportermover_t* mover) {
}
