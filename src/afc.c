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
 
 slightly cleaned up, got rid of the global variable, couldn't get it working to send / handle notifications through nos. those lines are commented out and if anyone wants to take a stab at it, by all means. this still at least works to send a file through afc which is its primary reason for existing.
 
 
 */

static void afc_free_dictionary(char **dictionary) //ghetto i know, not sure where/how to put a global function for this
{
	int i = 0;
	
	if (!dictionary)
		return;
	
	for (i = 0; dictionary[i]; i++) {
		free(dictionary[i]);
	}
	free(dictionary);
}

void apparition_afc_get_file_contents(afc_t* afc, const char *filename, char **data, uint64_t *size)
{
	if (!afc || !data || !size) {
		return;
	}
	
	char **fileinfo = NULL;
	uint32_t fsize = 0;
	
	afc_get_file_info(afc->client, filename, &fileinfo);
	if (!fileinfo) {
		return;
	}
	int i;
	for (i = 0; fileinfo[i]; i+=2) {
		if (!strcmp(fileinfo[i], "st_size")) {
			fsize = atol(fileinfo[i+1]);
			break;
		}
	}
	afc_free_dictionary(fileinfo);
	
	if (fsize == 0) {
		return;
	}
	
	uint64_t f = 0;
	afc_file_open(afc->client, filename, AFC_FOPEN_RDONLY, &f);
	if (!f) {
		return;
	}
	char *buf = (char*)malloc((uint32_t)fsize);
	uint32_t done = 0;
	while (done < fsize) {
		uint32_t bread = 0;
		afc_file_read(afc->client, f, buf+done, 65536, &bread);
		if (bread > 0) {
			
		} else {
			break;
		}
		done += bread;
	}
	if (done == fsize) {
		*size = fsize;
		*data = buf;
	} else {
		free(buf);
	}
	afc_file_close(afc->client, f);
}


static void perform_notification(idevice_t phone, lockdownd_client_t client, const char *notification)
{
	uint16_t nport = 0;
	np_client_t np;
	
	lockdownd_start_service(client, "com.apple.mobile.notification_proxy", &nport);
	if (nport) {
			//printf("::::::::::::::: np was started ::::::::::::\n");
		np_client_new(phone, nport, &np);
		if (np) {
				//printf("::::::::: PostNotification %s\n", notification);
			np_post_notification(np, notification);
			np_client_free(np);
		}
	} else {
			//	printf("::::::::::::::: np was NOT started ::::::::::::\n");
	}
}

afc_t* afc_open(nos_t* nos) {
	int err = 0;
	afc_t* afc = NULL;

	afc = (afc_t*) malloc(sizeof(afc_t));
	if(afc == NULL) {
		return NULL;
	}
	memset(afc, '\0', sizeof(afc_t));

	afc->notifier = nos;
	afc->lockdown = nos->lockdown;
	
	lockdown_t* lockdown = nos->lockdown;
	
	device_t *device = lockdown->device;
	uint16_t port = 0;
	uint16_t i = 0;
	uint16_t npp;
		// idevice_set_debug_level(1); //<-- if we ever want to enable debug logging
	
		// lockdown_start_service(afc->lockdown, "com.apple.afc", port); //CANT GET THIS WORKIN??? seems a bit overkill anyways.
	
	lockdownd_start_service(lockdown->client, "com.apple.afc", &port);
	if (port)
	{
		printf("opened afc successfully!\n");//debug
		afc->client = NULL;
		afc_client_new(device->client, port, &(afc->client));
		if ((afc->client)) {
			
				//used to watch and print notifiers here, not sure if anything will be necessary in the future
		}
	}
	
	return afc;
}


int afc_send_file(afc_t* afc, const char* local, const char* remote) {

	lockdown_t *ld = afc->lockdown;
	device_t *device = ld->device;
	nos_t *nos = afc->notifier;
	uint64_t lockfile = 0;
	uint64_t my_file = 0;
		unsigned int bytes = 0;
		perform_notification(device->client, ld->client, NP_SYNC_WILL_START);
		//nos_perform_notification(nos, NP_SYNC_WILL_START); //if used more then once it gets a broken pipe.
	
	afc_file_open(afc->client, "/com.apple.itunes.lock_sync", AFC_FOPEN_RW, &lockfile);
	if (lockfile) {
			//	printf("locking file\n");
		afc_file_lock(afc->client, lockfile, AFC_LOCK_EX);
		
			perform_notification(device->client, ld->client, NP_SYNC_DID_START);
			//nos_perform_notification(nos, NP_SYNC_DID_START); //if used more then once it gets a broken pipe.
	}
	
	
	afc_file_open(afc->client, remote, AFC_FOPEN_WR, &my_file);
	if (my_file) {
			//char *outdatafile = strdup("THIS IS HOW WE DO IT, WHORE\n");
		
			//FIXME: right here its just sending "local/file.txt, rather than the contents of the file.
		
			//afc_file_write(afc->client, my_file, outdatafile, strlen(outdatafile), &bytes);// <-- old code
		afc_file_write(afc->client, my_file, local, strlen(local), &bytes);
			//free(outdatafile);
		if (bytes > 0)
			printf("File transferred successfully\n");
		else
			printf("File write failed!!! :(\n");
		afc_file_close(afc->client, my_file);
	}
	
	
	
	if (lockfile) {
			//printf("XXX sleeping\n");
		sleep(5);
		
			//printf("XXX unlocking file\n");
		afc_file_lock(afc->client, lockfile, AFC_LOCK_UN);
		
			//	printf("XXX closing file\n");
		afc_file_close(afc->client, lockfile);
		
			//printf("XXX sleeping\n");
		sleep(5);

	}
	
	//if (gnp) {
//		np_client_free(gnp);
//		gnp = NULL;
//	}
	
		//afc_client_free(afc->client);
	
	
	printf("afc all done.\n");
	
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