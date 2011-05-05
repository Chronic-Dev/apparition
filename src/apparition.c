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

#include "backup.h"

int main(void) {
	int err = 0;

	// Create an empty backup_t object
	backup_t* backup = backup_create();
	if(backup == NULL) {
		printf("Unable to create backup object\n");
		return -1;
	}

	err = backup_add_file(backup, "./whatever", "/tmp/whatever");
	if(err < 0) {
		printf("Unable to add file to backup\n");
		backup_free(backup);
	}

	backup_free(backup);
	return 0;
}
