/*
 * mb2.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mb2.h"
#include "backup.h"
#include "lockdown.h"

mb2_t* mb2_open(lockdown_t* lockdown) {
	int err = 0;
	mb2_t* mb2 = NULL;
	
	mb2 = (mb2_t*) malloc(sizeof(mb2_t));
	if(mb2 == NULL) {
		return NULL;
	}
	memset(mb2, '\0', sizeof(mb2_t));
	
	mb2->lockdown = lockdown;

	return mb2;
}

int mb2_restore(mb2_t* mb2, backup_t* backup) {
	//TODO: Implement Me
	return -1;
}

int mb2_close(mb2_t* mb2) {
	//TODO: Implement Me
	return -1;
}

void mb2_free(mb2_t* mb2) {
	if(mb2) {
		free(mb2);
	}
}
