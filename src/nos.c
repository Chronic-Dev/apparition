/**
  * GreenPois0n Apparition - nos.c
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
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/notification_proxy.h>

#include "nos.h"
#include "lockdown.h"

#define ANP_SERVICE_NAME "com.apple.mobile.notification_proxy"

nos_t* nos_create() {
	nos_t* nos = (nos_t*) malloc(sizeof(nos_t));
	if (nos == NULL) {
		return NULL;
	}
	memset(nos, '\0', sizeof(nos_t));
	return nos;
}

nos_t* nos_open(device_t* device) {
	int err = 0;
	uint16_t port = 0;
	np_client_t np = NULL;
	np_error_t error = NP_E_SUCCESS;
	idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;

	// Sanity check arguments
	if(device == NULL || device->client == NULL) {
		printf("Unable to open connection to notification service due to invalid arguments\n");
		return NULL;
	}

	if(device->lockdown->nos == NULL) {
		device->lockdown->nos = nos_create();
		if(device->lockdown->nos == NULL) {
			fprintf(stderr, "Unable to create notification context\n");
			return NULL;
		}
	}

	lockdown_start_service(device->lockdown, ANP_SERVICE_NAME, &port);
	if (port == 0) {
		fprintf(stderr, "Unable to start notification sevice\n");
		return NULL;
	}
	lockdown_close(device->lockdown);

	error = np_client_new(device->client, port, &(device->lockdown->nos));
	if(error != NP_E_SUCCESS) {
		fprintf(stderr, "Unable to create new notification client\n");
		return NULL;
	}

	return device->lockdown->nos;
}

int nos_register(nos_t* nos, nos_cb_t callback, void *arg) {
	if (!nos || !callback) {
		printf("ERROR: missing or invalid parameters!\n");
		return -1;
	}

	if (np_set_notify_callback(nos->client, callback, arg) != NP_E_SUCCESS) {
		printf("ERROR: could not set notification callback!\n");
		return -1;
	}
	const char *noties[6] = { NP_SYNC_CANCEL_REQUEST,
				NP_SYNC_SUSPEND_REQUEST, NP_SYNC_RESUME_REQUEST,
				NP_BACKUP_DOMAIN_CHANGED, NP_ITDBPREP_DID_END, NULL };
	if (np_observe_notifications(nos->client, noties) != NP_E_SUCCESS) {
		printf("ERROR: could not register for notification observation!\n");
		np_set_notify_callback(nos->client, NULL, NULL);
		return -1;
	} 
	return 0;
}

void nos_perform_notification(nos_t* nos, const char *notification) {
	if (!nos || !notification) {
		printf("ERROR: missing or invalid parameters!\n");
		return -1;
	}
	printf("::::::::: PostNotification %s\n", notification);
	np_post_notification(nos->client, notification);
}

void nos_free(nos_t* nos) {
	if (nos) {
		if (nos->client) {
			np_client_free(nos->client);
		}
		free(nos);
	}
}

/*
 
 static void notify_cb(const char *notification, void *userdata)
 {
 if (!strcmp(notification, NP_SYNC_CANCEL_REQUEST)) {
 printf("User has cancelled the backup process on the device.\n");

 } else {
 printf("Unhandled notification '%s' (TODO: implement)\n", notification);
 }
 }
 
 */

/*

 
 

 */
