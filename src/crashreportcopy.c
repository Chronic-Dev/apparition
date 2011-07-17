/**
  * GreenPois0n Apparition - crashreportcopy.c
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
#include "crashreportcopy.h"

crashreportcopy_t* crashreportcopy_create() {
	crashreportcopy_t* copier = (crashreportcopy_t*) malloc(sizeof(crashreportcopy_t));
	if(copier) {
		memset(copier, '\0', sizeof(crashreportcopy_t));
	}
	return copier;
}

void crashreportcopy_free(crashreportcopy_t* copier) {
	if(copier) {
		if(copier->connection) {
			crashreportcopy_close(copier);
		}
		free(copier);
	}
}

crashreportcopy_t* crashreportcopy_open(device_t* device) {
	int err = 0;
	unsigned short port = 0;
	crashreportcopy_t* copier = crashreportcopy_create();
	if(copier == NULL) {
		printf("Unable to open crashreport copy service\n");
		return NULL;
	}
	
	err = lockdownd_start_service(device->lockdown->client, "com.apple.crashreportcopymobile", &(copier->port));
	if(err < 0 ) {
		return NULL;
	}

	return copier;
}

int crashreportcopy_close(crashreportcopy_t* copier) {
	return -1;
}
