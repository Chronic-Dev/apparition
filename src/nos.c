/*
 * nos.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/notification_proxy.h>
#include <libimobiledevice/lockdown.h>

#include "nos.h"

#define ANP_SERVICE_NAME "com.apple.mobile.notification_proxy"


nos_t* nos_open(lockdown_t* lockdown) {
		int err = 0;
		nos_t* nos = NULL;

		nos = (nos_t*) malloc(sizeof(nos_t));
		if(nos == NULL) {
			return NULL;
		}
	
		//start notification_proxy 
	
	
	
		memset(nos, '\0', sizeof(nos_t));

		nos->lockdown = lockdown;

		return nos;
}

int nos_register(nos_t* nos, np_notify_cb_t callback, idevice_t phone) {
	
	 idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
	uint16_t port = 0;
	np_client_t np = NULL;
	lockdown_t *ld = nos->lockdown;
	ret = lockdownd_start_service(ld->client, ANP_SERVICE_NAME, &port);
	if ((ret == LOCKDOWN_E_SUCCESS) && port) {
		np_client_new(phone, port, &np);
		np_set_notify_callback(np, callback, NULL);
		const char *noties[5] = {
			NP_SYNC_CANCEL_REQUEST,
			NP_SYNC_SUSPEND_REQUEST,
			NP_SYNC_RESUME_REQUEST,
			NP_BACKUP_DOMAIN_CHANGED,
			NP_ITDBPREP_DID_END,
			NULL
		};
		np_observe_notifications(np, noties);
	} else {
		printf("ERROR: Could not start service %s.\n", ANP_SERVICE_NAME);
		return -1;
	}
	return nos;
}

void nos_free(nos_t* nos) {
	if(nos) {
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