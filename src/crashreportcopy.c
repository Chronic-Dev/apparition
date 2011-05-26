/*
 * crashreportcopy.c
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#include <stdlib.h>
#include <stdio.h>
#include "crashreportcopy.h"
#include "device.h"
#include <string.h>


crashreportcopy_t* crashreportcopy_open(lockdown_t* lockdown) {
	int err = 0;
	unsigned short port = 0;
	crashreportcopy_t* copier = crashreportcopy_create(lockdown);
	
	err = lockdownd_start_service(lockdown->client, "com.apple.crashreportcopymobile", &(copier->port));
	if(err < 0 ) {
		return NULL;
	}

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
