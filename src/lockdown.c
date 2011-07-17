/**
 * GreenPois0n Apparition - lockdown.c
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
#include <libimobiledevice/lockdown.h>

#include "afc.h"
#include "mb2.h"
#include "nos.h"
#include "device.h"
#include "lockdown.h"

lockdown_t* lockdown_create() {
	lockdown_t* lockdown = (lockdown_t*) malloc(sizeof(lockdown_t));
	if (lockdown) {
		memset(lockdown, '\0', sizeof(lockdown_t));

		// Create our service structures. We allocate all these in the beginning to save time
		//   rather then allocating them lazily. Classic memory vs speed trade-off.
		// AFC Service
		lockdown->afc = afc_create();
		if (lockdown->afc == NULL) {
			printf("Unable to create lockdown context\n");
			lockdown_free(lockdown);
			return NULL;
		}

		// MobileBackup2 Service
		lockdown->mb2 = mb2_create();
		if (lockdown->mb2 == NULL) {
			printf("Unable to create lockdown context\n");
			lockdown_free(lockdown);
			return NULL;
		}

		// Notification Service
		lockdown->nos = nos_create();
		if (lockdown->nos == NULL) {
			printf("Unable to create lockdown context\n");
			lockdown_free(lockdown);
			return NULL;
		}

		// Crashreporter Service
		lockdown->crashreporter = crashreporter_create();
		if (lockdown->crashreporter == NULL) {
			printf("Unable to create lockdown context\n");
			lockdown_free(lockdown);
			return NULL;
		}
	}

	return lockdown;
}

lockdown_t* lockdown_open(device_t* device) {
	lockdownd_error_t err = 0;

	// Sanity check as always
	if (device == NULL) {
		printf("Unable to open lockdown daemon due to invalid arguments\n");
		return NULL;
	}
	lockdown_t* lockdown = device->lockdown;

	// Check and see if our lockdown context is already allocated
	if (lockdown == NULL) {
		// Lockdown context hasn't been allocated yet, so allocate it now
		lockdown = lockdown_create();
		if (lockdown == NULL) {
			// We had an error allocating memory for lockdown context =(
			printf("Unable to open lockdown client\n");
			return NULL;

		}
	}
	device->lockdown = lockdown;

	// Check and see if we're already connected to lockdown or not
	if (lockdown->client == NULL) {
		// Open the device connection and perform the handshake if possible.
		if (lockdownd_client_new_with_handshake(device->client,
				&(lockdown->client), "apparition") != LOCKDOWN_E_SUCCESS) {
			// Error creating client or during handshake, cleanup self and return error
			printf("Unable to pair with lockdown\n");
			lockdown_free(device->lockdown);
			device->lockdown == NULL;
			return NULL;
		}
	} else {
		// We're already connected to lockdown, so why are we here???
		// TODO: Should we go ahead and close and reallocate memory for
		//   a new lockdown connection or just return the context
		//   we have?
		printf("Connection to lockdown client is already open...\n");
	}

	// Copy our pointer to the device we're opening a connection to
	lockdown->device = device;
	return lockdown;
}

int lockdown_close(lockdown_t* lockdown) {
	lockdownd_error_t error = 0;

	// Sanity check the arguments and make sure we're really trying to close something
	if (lockdown == NULL) {
		printf("Unable to close lockdown connection due to invalid arguments\n");
		return -1;
	}

	// Check and make sure we really have a connection to close down
	if (lockdown->client == NULL) {
		printf(
				"Unable to close lockdown connection because it's not even open... wtf are you doing?\n");
		return -1;
	}

	// Tell lockdownd goodbye for now, we're all through with you
	error = lockdownd_goodbye(lockdown->client);
	if (error != LOCKDOWN_E_SUCCESS) {
		printf("Unable to close lockdown connection,  error %d\n", error);
		return -1;
	}

	// Clean up after ourselves once we're done
	lockdownd_client_free(lockdown->client);
	lockdown->client = NULL;
	return 0;
}

void lockdown_free(lockdown_t* lockdown) {
	// Sanity check the arguments and make sure we're really trying to free something
	if (lockdown) {
		// Shutdown any services still open before freeing our lockdown context
		// If afc context exists then free it and it's substructures
		if (lockdown->afc) {
			afc_free(lockdown->afc);
			lockdown->afc = NULL;
		}

		// If mb2 context exists then free it and it's substructures
		if (lockdown->mb2) {
			mb2_free(lockdown->mb2);
			lockdown->mb2 = NULL;
		}

		// If nos context exists then free it and it's substructures
		if (lockdown->nos) {
			nos_free(lockdown->nos);
			lockdown->nos = NULL;
		}

		// If crashreporter context exists then free it and it's substructures
		if (lockdown->crashreporter) {
			crashreporter_free(lockdown->crashreporter);
			lockdown->crashreporter = NULL;
		}

		// Close our lockdown client, this should free and NULL it as well
		if (lockdown->client) {
			lockdown_close(lockdown);
			// But we'll do it again anyways =P
			lockdown->client = NULL;
		}

		lockdown->device = NULL;
		free(lockdown);
	}
}

int lockdown_start_service(lockdown_t* lockdown, const char* service,
		uint16_t* port) {
	// Sanity check our arguments, and make sure a lockdown connection exists
	//  by checking the client isn't null
	if (lockdown == NULL || service == NULL || port == NULL) {
		printf("Unable to start lockdown service due to invalid arguments\n");
		return -1;
	}

	// Make sure our lockdown client exists before trying to do anything with it
	if (lockdown->client == NULL) {
		printf(
				"Unable to start service for lockdown because it hasn't been opened\n");
		return -1;
	}

	// TODO: What exactly is this suppose to return? Does anyone know?
	lockdownd_start_service(lockdown->client, service, port);
	if (lockdown->client == NULL) {
		printf("Unable to start lockdownd service\n");
		return -1;
	}

	// Check and make sure lockdown returned us our service port for connection
	if (*port == 0) {
		// Port is 0, something obviously fucked up
		printf("Unable to start service %s!\n", service);
		return -1;
	}

	// We made it this far, the service should be up and running and waiting for our connection
	printf("Successfully started %s on port %d!\n", service, *port);
	return 0;
}

int lockdown_stop_service(lockdown_t* lockdown, const char* service) {
	// Sanity check our arguments, and make sure a lockdown connection exists
	//  by checking the client isn't null
	if (lockdown == NULL || service == NULL || lockdown->client == NULL) {
		printf("Unable to stop lockdown service due to invalid arguments\n");
		return -1;
	}

	// TODO: How should we go about stopping services through lockdown?
	//lockdownd_stop_session();

	// We made it this far, the service should be up and running and waiting for our connection
	printf("Successfully stopped %s\n", service);
}
