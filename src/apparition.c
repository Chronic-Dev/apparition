/*
 ============================================================================
 Name        : apparition.c
 Author      : Chronic-Dev Team
 Version     :
 Copyright   : (c) 2011 Chronic-Dev, LLC
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/* This is not only the entry point, but since this is a console application
 *  this is also the applications View. No other file in this project is allowed
 *  to print to stdout other then this file. You may print to stderr in the case
 *  of a non-recoverable error, or for debugging purposes (must be removed for release)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afc.h"
#include "mb2.h"
#include "nos.h"
#include "mbdb.h"
#include "mbdx.h"
#include "backup.h"
#include "device.h"
#include "lockdown.h"
#include "apparition.h"
#include "mbdx_record.h"
#include "mbdb_record.h"

static void notify_cb(const char *notification, void *userdata)
{
	if (!strcmp(notification, NP_SYNC_CANCEL_REQUEST)) {
		printf("User has cancelled the backup process on the device.\n");
	} else {
		printf("Unhandled notification '%s' (TODO: implement)\n", notification);
	}
}

int main(int argc, char* argv[]) {
	int err = 0;
	mbdx_record_t* mbdx_record = NULL;
	mbdb_record_t* mbdb_record = NULL;

	// Pass a UUID here if you want to target a single device,
	//  or NULL to select the first one it finds
	printf("Openning device connection\n");
	device_t* device = device_create(NULL);
	if(device == NULL) {
		printf("Unable to find a device to use\n");
		return -1;
	}
	device_enable_debug();

	// Create our fake backup
	// Create an empty backup_t object
	printf("Opening backup directory\n");
	backup_t* backup = backup_open("Backup", device->uuid);
	if(backup == NULL) {
		printf("Unable to create backup object\n");
		return -1;
	}

	/*
	backup_file_t* file = backup_file_create();
	if(file == NULL) {
		printf("Unable to create backup file\n");
		backup_free(backup);
		return -1;
	}
	
	file->mbdx_record = mbdx_record_create(backup->mbdx);
	file->mbdb_record = mbdb_record_create();

	mbdb_record = file->mbdb_record;
	mbdb_record->target = "";
	mbdb_record->domain = "MediaDomain";
	mbdb_record->path = "";
	mbdb_record->length = 0;
	mbdb_record->mode = 0;
	mbdb_record->gid = 0;
	mbdb_record->uid = 0;

	// Append a file to the current backup object
	err = backup_add_file(backup, file);
	if(err < 0) {
		printf("Unable to add file to backup\n");
			//backup_free(backup);
	}
	*/

	printf("Saving new backup directory\n");
	mkdir("Clone", 0644);
	mkdir("Clone/2e284f1a9bdc8be302d43f935784a1a5cc66fa78", 0644);
	err = backup_save(backup, "Clone", "2e284f1a9bdc8be302d43f935784a1a5cc66fa78");
	if(err < 0) {
		printf("Unable to save backup\n");
		return -1;
	}

	// Open connection with the lockdownd service daemon
	printf("Initializing lockdown service daemon\n");
	// TODO: Change this to lockdown_init and allow afc_t and nos_t to call lockdown_open theirself
	lockdown_t* lockdown = lockdown_open(device);
	if(lockdown == NULL) {
		printf("Unable to connect to lockdownd\n");
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}

	printf("Openning connection to notification service\n");
	nos_t* nos = nos_open(lockdown);
	if (nos == NULL) {
		printf("Unable to open notification center!!\n");
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}

	printf("Registering notification callbacks\n");
	err = nos_register(nos, notify_cb, device->client);
	if(err < 0) {
		printf("Unable to register for notification callback!!\n");
		nos_free(nos);
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}

	// Open and initialize the afc connection
	printf("Opening connection to AFC\n");
	afc_t* afc = afc_open(nos);
	if(afc == NULL) {
		printf("Unable to open connection to afc service\n");
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}

	// Send a file from your computer to the device
	printf("Sending file over AFC\n");
	err = afc_send_file(afc, "local/file.txt", "/file.txt");
	if(err < 0) {
		printf("Unable to send file over apple file conduit\n");
		afc_free(afc);
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}
	// Close our connection, but don't free our object
	afc_close(afc);

	// Open and initialize the mb2 connection
	printf("Opening connection to backup service\n");
	mb2_t* mb2 = mb2_open(lockdown, afc);
	if(mb2 == NULL) {
		printf("Unable to open connection to mobilebackup2 service");
		afc_free(afc);
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
	}

	// Perform a restore from a backup object
	printf("Performing restore from backup\n");
	err = mb2_restore(mb2, backup);
	if(err < 0) {
		printf("Unable to restore our backup object\n");
		mb2_free(mb2);
		afc_free(afc);
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
	}
	mb2_close(mb2);

	printf("Cleaning up\n");
	// If open, then close and free structures
	if(mb2) mb2_free(mb2);
	if(afc) afc_free(afc);
	if(lockdown) lockdown_free(lockdown);
	if(device) device_free(device);
	if(backup) backup_free(backup);
	printf("Done\n");
	return 0;
}
