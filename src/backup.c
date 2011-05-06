/*
 * backup.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

/* This is the application Model
 *  Functions in this file should handle openning, reading, and parsing
 *  backup directories. it should allow access to mbdb and mbdx, as well
 *  as extract and injecting files into the databases.
 */

#include <stdio.h>

#include "backup.h"

backup_t* backup_create() {
	//TODO: Implement Me
	return NULL;
}

void backup_free(backup_t* backup) {
	//TODO: Implement Me
}

int backup_add_file(backup_t* backup, const char* local, const char* remote){
	//TODO: Implement Me
	return -1;
}
