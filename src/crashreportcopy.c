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

#include <stdlib.h>
#include <stdio.h>
#include "crashreportcopy.h"
#include "device.h"
#include <string.h>


crashreportcopy_t* crashreportcopy_open(lockdown_t* lockdown) {
	int err = 0;
	unsigned short port = 0;
	crashreportcopy_t* copier = crashreportcopy_create(lockdown);
	
	err = lockdownd_start_service(lockdown->client, "com.apple.crashreportcopymobile", &(copier->port));
	if(err < 0 ) {
		return NULL;
	}

	return copier;
}

crashreportcopy_t* crashreportcopy_create(lockdown_t* lockdown) {
	crashreportcopy_t* copier = NULL;
	if(lockdown == NULL) {
		printf("Unable to create crashreportercopier object\n");
		return NULL;
	}
	
	copier = (crashreportcopy_t*) malloc(sizeof(crashreportcopy_t));
	if(copier == NULL) {
		return NULL;
	}
	
	return copier;
}

int crashreportcopy_close(crashreportcopy_t* copier) {
	return -1;
}

void crashreportcopy_free(crashreportcopy_t* copier) {
}
