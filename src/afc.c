/*
 * afc.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/afc.h>

#include "afc.h"
#include "device.h"
#include "lockdown.h"
#include "nos.h"

/*
 
 this code was mostly plucked and tweaked from imobiledevice/dev/ideviceclient, it will be modified tomorrow to be *proper* and 
 use nos and to not have any global variables, i just wanted to get it working first and then make it proper after i had
 a better understanding of how it worked, right now it is the same as it was in ideviceclient and is hardcoded to send
 a specifc file that we make from scratch, will finish it tomorrow and also make it close properly from the close function,
 
 please don't delete this code because it isn't proper, it will be fixed tomorrow.
 
 
 */

static np_client_t gnp = NULL; //will be excised in the future for nos, as will the functions below.

static void notifier(const char *notification, void *userdata)
{
	printf("---------------------------------------------------------\n");
	printf("------> Notification received: %s\n", notification);
	printf("---------------------------------------------------------\n");
}

static void perform_notification(idevice_t phone, lockdownd_client_t client, const char *notification)
{
	uint16_t nport = 0;
	np_client_t np;
	
	lockdownd_start_service(client, "com.apple.mobile.notification_proxy", &nport);
	if (nport) {
		printf("::::::::::::::: np was started ::::::::::::\n");
		np_client_new(phone, nport, &np);
		if (np) {
			printf("::::::::: PostNotification %s\n", notification);
			np_post_notification(np, notification);
			np_client_free(np);
		}
	} else {
		printf("::::::::::::::: np was NOT started ::::::::::::\n");
	}
}

afc_t* afc_open(lockdown_t* lockdown) {
	int err = 0;
	afc_t* afc = NULL;

	afc = (afc_t*) malloc(sizeof(afc_t));
	if(afc == NULL) {
		return NULL;
	}
	memset(afc, '\0', sizeof(afc_t));

	afc->lockdown = lockdown;

	
	device_t *device = lockdown->device;
	uint16_t port = 0;
	uint16_t i = 0;
	uint16_t npp;


	
		// idevice_set_debug_level(1); //<-- if we ever want to enable debug logging
	
	
		// lockdown_start_service(afc->lockdown, "com.apple.afc", port); //CANT GET THIS WORKIN??? seems a bit overkill anyways.
	
	lockdownd_start_service(lockdown->client, "com.apple.afc", &port);
	if (port)
	{
		printf("opened afc successfully!\n");
	}
	
	if (port) {
		afc->client = NULL;
		afc_client_new(device->client, port, &(afc->client));
		if ((afc->client)) {
			lockdownd_start_service(lockdown->client, "com.apple.mobile.notification_proxy", &npp);
			if (npp) {
				printf("Notification Proxy started.\n");
				np_client_new(device->client, npp, &gnp);
			} else {
				printf("ERROR: Notification proxy could not be started.\n");
			}
			if (gnp) {
				const char *nspec[5] = {
					NP_SYNC_CANCEL_REQUEST,
					NP_SYNC_SUSPEND_REQUEST,
					NP_SYNC_RESUME_REQUEST,
					NP_ITDBPREP_DID_END,
					NULL
				};
				np_observe_notifications(gnp, nspec);
				np_set_notify_callback(gnp, notifier, NULL);
			}
		}
	}
	return afc;
}


int afc_send_file(afc_t* afc, const char* local, const char* remote) {
		//TODO: Implement Me
	lockdown_t *ld = afc->lockdown;
	device_t *device = ld->device;
	uint64_t lockfile = 0;
	uint64_t my_file = 0;
		unsigned int bytes = 0;
	perform_notification(device->client, ld->client, NP_SYNC_WILL_START);
	
	afc_file_open(afc->client, "/com.apple.itunes.lock_sync", AFC_FOPEN_RW, &lockfile);
	if (lockfile) {
		printf("locking file\n");
		afc_file_lock(afc->client, lockfile, AFC_LOCK_EX);
		
		perform_notification(device->client, ld->client, NP_SYNC_DID_START);
	}
	
	
	afc_file_open(afc->client, "/apparition.gp.pwnage", AFC_FOPEN_WR, &my_file);
	if (my_file) {
		char *outdatafile = strdup("THIS IS HOW WE DO IT, WHORE\n");
		afc_file_write(afc->client, my_file, outdatafile, strlen(outdatafile), &bytes);
		free(outdatafile);
		if (bytes > 0)
			printf("File transferred successfully\n");
		else
			printf("File write failed!!! :(\n");
		afc_file_close(afc->client, my_file);
	}
	
	
	
	if (gnp && lockfile) {
		printf("XXX sleeping\n");
		sleep(5);
		
		printf("XXX unlocking file\n");
		afc_file_lock(afc->client, lockfile, AFC_LOCK_UN);
		
		printf("XXX closing file\n");
		afc_file_close(afc->client, lockfile);
		
		printf("XXX sleeping\n");
		sleep(5);
			//perform_notification(phone, client, NP_SYNC_DID_FINISH);
	}
	
	if (gnp) {
		np_client_free(gnp);
		gnp = NULL;
	}
	
		//afc_client_free(afc->client);
	
	
	printf("All done.\n");
	
	return 0;
}


int afc_close(afc_t* afc) {
	//TODO: Implement Me
	return -1;
}

void afc_free(afc_t* afc) {
	if(afc) {
		free(afc);
	}
}



/*

 plucked right from: libimobiledevice/dev/ideviceclient.c
 


 

 

*/