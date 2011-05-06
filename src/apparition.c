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

int main(int argc, char* argv[]) {
	int err = 0;

	// First step is to create our fake backup
	// Create an empty backup_t object
	backup_t* backup = backup_create();
	if(backup == NULL) {
		printf("Unable to create backup object\n");
		return -1;
	}

	// Append a file to the current backup object
	err = backup_add_file(backup, "./whatever", "/tmp/whatever");
	if(err < 0) {
		printf("Unable to add file to backup\n");
		backup_free(backup);
	}

	// Now we need to
	// Pass a UUID here if you want to target a single device,
	//  or NULL to select the first one it finds
	device_t* device = device_create(NULL);
	if(device == NULL) {
		printf("Unable to find a device to use\n");
		backup_free(backup);
		return -1;
	}

	// Open connection with the lockdownd service daemon
	lockdown_t* lockdown = lockdown_open(device);
	if(lockdown == NULL) {
		printf("Unable to connect to lockdownd\n");
		backup_free(backup);
		return -1;
	}

	// Open and initialize the afc connection
	afc_t* afc = afc_open(lockdown);
	if(afc == NULL) {
		printf("Unable to open connection to afc service\n");
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
		return -1;
	}

	// Send a file from your computer to the device
	err = afc_send_file(afc, "local/file.txt", "remote/file.txt");
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
	mb2_t* mb2 = mb2_open(lockdown);
	if(mb2 == NULL) {
		printf("Unable to open connection to mobilebackup2 service");
		afc_free(afc);
		lockdown_free(lockdown);
		device_free(device);
		backup_free(backup);
	}

	// Perform a restore from a backup object
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

	// If open, then close and free structures
	if(mb2) mb2_free(mb2);
	if(afc) afc_free(afc);
	if(lockdown) lockdown_free(lockdown);
	if(device) device_free(device);
	if(backup) backup_free(backup);
	return 0;
}
