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
#include "device.h"
#include "lockdown.h"

#define ANP_SERVICE_NAME "com.apple.mobile.notification_proxy"

nos_t* nos_create(lockdown_t* lockdown) {
	int err = 0;
	nos_t* nos = NULL;

	nos = (nos_t*) malloc(sizeof(nos_t));
	if (nos == NULL) {
		return NULL;
	}

	//start notification_proxy
	memset(nos, '\0', sizeof(nos_t));

	nos->lockdown = lockdown;
	return nos;
}

nos_t* nos_open(lockdown_t* lockdown) {
	uint16_t port = 0;
	np_client_t np = NULL;
	np_error_t nos_error = NP_E_SUCCESS;
	idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
	if(lockdown->nos == NULL) {
		lockdown->nos = nos_create(lockdown);
		if(lockdown->nos == NULL) {
			return NULL;
		}
	}

	ret = lockdownd_start_service(lockdown->client, ANP_SERVICE_NAME, &port);
	if ((ret != LOCKDOWN_E_SUCCESS) && port) {
		fprintf(stderr, "Unable to start notification sevice\n");
		return NULL;
	}

	nos_error = np_client_new(lockdown->device->client, port, &(lockdown->nos->client));
	if(nos_error != NP_E_SUCCESS) {
		fprintf(stderr, "Unable to create new notification client\n");
		return NULL;
	}
	return lockdown->nos;
}

int nos_register(nos_t* nos, np_notify_cb_t callback, idevice_t phone) {
	uint16_t port = 0;
	np_client_t np = NULL;
	lockdown_t *ld = nos->lockdown;
	idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;

	ret = lockdownd_start_service(ld->client, ANP_SERVICE_NAME, &port);
	if ((ret == LOCKDOWN_E_SUCCESS) && port) {
		np_client_new(phone, port, &(nos->client));
		np_set_notify_callback(nos->client, callback, NULL);
		const char *noties[6] = { NP_SYNC_CANCEL_REQUEST,
				NP_SYNC_SUSPEND_REQUEST, NP_SYNC_RESUME_REQUEST,
				NP_BACKUP_DOMAIN_CHANGED, NP_ITDBPREP_DID_END, NULL };
		np_observe_notifications(nos->client, noties);
	} else {
		printf("ERROR: Could not start service %s.\n", ANP_SERVICE_NAME);
		return -1;
	}
	return 0;
}

void nos_perform_notification(nos_t* nos, const char *notification) {
	printf("::::::::: PostNotification %s\n", notification);
	np_post_notification(nos->client, notification);
	//np_client_free(np);


}

void nos_free(nos_t* nos) {
	if (nos) {
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
