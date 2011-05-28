/*
 * nos.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/notification_proxy.h>

#include "nos.h"
#include "lockdown.h"

#define ANP_SERVICE_NAME "com.apple.mobile.notification_proxy"

nos_t* nos_open(lockdown_t* lockdown) {
	uint16_t port = 0;
	np_client_t np = NULL;
	np_error_t nos_error = NP_E_SUCCESS;
	idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;

	lockdown_start_service(lockdown, ANP_SERVICE_NAME, &port);
	if (!port) {
		fprintf(stderr, "Unable to start notification sevice\n");
		return NULL;
	}

	nos_error = np_client_new(lockdown->device->client, port, &np);
	if(nos_error != NP_E_SUCCESS) {
		fprintf(stderr, "Unable to create new notification client\n");
		return NULL;
	}

	nos_t* nos = NULL;

	nos = (nos_t*) malloc(sizeof(nos_t));
	if (nos == NULL) {
		return NULL;
	}

	memset(nos, '\0', sizeof(nos_t));
	nos->client = np;
	nos->lockdown = lockdown;

	return nos;
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
