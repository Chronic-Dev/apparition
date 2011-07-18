/**
  * GreenPois0n Apparition - crashreportmover.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "lockdown.h"
#include "crashreportmover.h"

crashreportmover_t* crashreportmover_create() {
	printf(">> %s called\n", __func__);
	crashreportmover_t* mover = (crashreportmover_t*) malloc(sizeof(crashreportmover_t));
	if(mover) {
		memset(mover, '\0', sizeof(crashreportmover_t));
	}
	return mover;
}

void crashreportmover_free(crashreportmover_t* mover) {
	printf(">> %s called\n", __func__);
	if(mover) {
		if(mover->connection) {
			crashreportmover_close(mover);
		}
		free(mover);
	}
}

crashreportmover_t* crashreportmover_open(device_t* device) {
	printf(">> %s called\n", __func__);
	int err = 0;
	unsigned short port = 0;
	crashreportmover_t* mover = crashreportmover_create(device);
	if(mover == NULL) {
		printf("Unable to open crashreporter move service\n");
		return NULL;
	}
	
	err = lockdownd_start_service(device->lockdown->client, "com.apple.crashreportmover", &port);
	if(err < 0 ) {
		return NULL;
	}
	
	mover->port = port;

	err = idevice_connect(device->client, port, &(mover->connection));
	if(err < 0) {
		return NULL;
	}
	idevice_disconnect(mover->connection);
	return mover;
}



int crashreportmover_close(crashreportmover_t* mover) {
	printf(">> %s called\n", __func__);
	return 0;
}
