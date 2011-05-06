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

#include "nos.h"
#include "lockdown.h"

nos_t* nos_open(lockdown_t* lockdown) {
		int err = 0;
		nos_t* nos = NULL;

		nos = (nos_t*) malloc(sizeof(nos_t));
		if(nos == NULL) {
			return NULL;
		}
		memset(nos, '\0', sizeof(nos_t));

		nos->lockdown = lockdown;

		return nos;
}

int nos_register(nos_t* nos, char* callback, void* whatever) {
	//TODO: Implement Me
	return -1;
}

void nos_free(nos_t* nos) {
	if(nos) {
		free(nos);
	}
}
