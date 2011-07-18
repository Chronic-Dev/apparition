/**
  * GreenPois0n Apparition - crashreportcopy.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afc.h"
#include "device.h"
#include "lockdown.h"
#include "crashreportcopy.h"

crashreportcopy_t* crashreportcopy_create() {
	printf(">> %s called\n", __func__);
	crashreportcopy_t* copier = (crashreportcopy_t*) malloc(sizeof(crashreportcopy_t));
	if(copier) {
		memset(copier, '\0', sizeof(crashreportcopy_t));

		// Crashreporter borrows the afc client for communication with the copier
		copier->afc = afc_create();
		if(copier->afc == NULL) {
			crashreportcopy_free(copier);
			return NULL;
		}
	}
	return copier;
}

void crashreportcopy_free(crashreportcopy_t* copier) {
	printf(">> %s called\n", __func__);
	if(copier) {
		if(copier->connection) {
			crashreportcopy_close(copier);
		}
		free(copier);
	}
}

crashreportcopy_t* crashreportcopy_open(device_t* device) {
	printf(">> %s called\n", __func__);
	int err = 0;
	uint16_t port = 0;

	// Sanity check to make sure we were passed a device
	if(device == NULL) {
		printf("Unable to start crashreport copier due to invalid arguments\n");
		return NULL;
	}

	// If lockdown isn't already open, then go ahead and open it
	if(device->lockdown == NULL) {
		device->lockdown = lockdown_open(device);
		if(device->lockdown == NULL) {
			printf("Unable to open lockdown connection for crashreports copy service\n");
			return NULL;
		}
	}

	// Finally check and make sure crashreporter context already exists
	if(device->lockdown->crashreporter == NULL) {
		printf("Unable to find crashreporter context\n");
		return NULL;
	}

	// This should of been created in crashreporter_create()
	crashreportcopy_t* copier = device->lockdown->crashreporter->copier;
	if(copier == NULL) {
		printf("Unable to open crashreport copy service\n");
		return NULL;
	}
	
	err = lockdown_start_service(device->lockdown, "com.apple.crashreportcopymobile", &(copier->port));
	if(err < 0 ) {
		return NULL;
	}

	return copier;
}

int crashreportcopy_close(crashreportcopy_t* copier) {
	printf(">> %s called\n", __func__);
	return 0;
}
