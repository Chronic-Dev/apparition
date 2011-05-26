/*
 * crashreportcopy.c
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#include <stdlib.h>
#include "crashreportcopy.h"


crashreportcopy_t* crashreportcopy_open(lockdown_t* lockdown) {
	int err = 0;
	unsigned short port = 0;
	crashreportcopy_t* copier = NULL;
	
	err = lockdown_start_service(lockdown, "com.apple.crashreportcopymobile", &port);
	if(err < 0 ) {
		return NULL;
	}
	
	err = idevice_connect(lockdown->device, copier->port, copier->connection);
	if(err < 0) {
		return NULL;
	}
	idevice_disconnect(copier->connection);
	return copier;
}

crashreportcopy_t* crashreportcopy_create(lockdown_t* lockdown) {
	crashreportcopy_t* copier = NULL;
	if(lockdown == NULL) {
		printf("Unable to create crashreportercopier object\n");
		return NULL;
	}
	
	copier = (crashreportcopy_t*) malloc(sizeof(crashreportcopy_t));
	if(copier == NULL) {
		return NULL;
	}
	
	return copier;
}

int crashreportcopy_close(crashreportcopy_t* copier) {
	return -1;
}

void crashreportcopy_free(crashreportcopy_t* copier) {
}
