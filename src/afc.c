/**
  * GreenPois0n Apparition - afc.c
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
#include <libimobiledevice/afc.h>

#include "afc.h"
#include "device.h"
#include "lockdown.h"

#define AFC_SERVICE "com.apple.afc"

afc_t* afc_create() {
	printf(">> %s called\n", __func__);

	afc_t* afc = (afc_t*) malloc(sizeof(afc_t));
	if (afc) {
		memset(afc, '\0', sizeof(afc_t));
	}

	return afc;
}

void afc_free(afc_t* afc) {
	printf(">> %s called\n", __func__);
	if (afc) {
		if(afc->connection) {
			afc_close(afc);
		}
		free(afc);
	}
}

afc_t* afc_open(device_t* device) {
	printf(">> %s called\n", __func__);

	int err = 0;
	afc_t* afc = NULL;
	lockdown_t* lockdown = NULL;

	if(device == NULL) {
		printf("Unable to open afc server due to invalid arguments\n");
		return NULL;
	}

	afc = device->lockdown->afc;
	if(afc) {
		memset(afc, '\0', sizeof(afc_t));
		err = afc_connect(afc, 0);
		if(err < 0) {
			printf("Unable to connect to afc port\n");
			return NULL;
		}
	}

	return afc;
}


int afc_connect(afc_t* afc, uint16_t port) {
	printf(">> %s called\n", __func__);

	int err = 0;
	//uint16_t port = 0;
	device_t* device = afc->device;

	/*
	afc_t* afc = afc_open(device);
	if(afc == NULL) {
		return NULL;
	}

	err = lockdown_start_service(device->lockdown, AFC_SERVICE, &port);
	if (err < 0 || port == 0) {
		printf("Unable to start AFC service\n");
		afc_free(afc);
		return NULL;
	}

	afc_client_new(device->client, afc->port, &(afc->client));
	if (afc->client == NULL) {
		printf("Unable to open connection to AFC client\n");
		afc_free(afc);
		return NULL;
	}
*/
	return 0;
}

int afc_close(afc_t* afc) {
	printf(">> %s called\n", __func__);
	//TODO: Implement Me
	return 0;
}

int afc_send_file(afc_t* afc, const char* local, const char* remote) {
	printf(">> %s called\n", __func__);
	uint64_t lockfile = 0;
	uint64_t my_file = 0;
	unsigned int bytes = 0;

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

	printf("afc all done.\n");

	return 0;
}

static void afc_free_dictionary(char **dictionary) //ghetto i know, not sure where/how to put a global function for this
{
	printf(">> %s called\n", __func__);
	int i = 0;

	if (!dictionary)
		return;

	for (i = 0; dictionary[i]; i++) {
		free(dictionary[i]);
	}
	free(dictionary);
}

void apparition_afc_get_file_contents(afc_t* afc, const char *filename,
		char **data, uint64_t *size) {
	printf(">> %s called\n", __func__);
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
	for (i = 0; fileinfo[i]; i += 2) {
		if (!strcmp(fileinfo[i], "st_size")) {
			fsize = atol(fileinfo[i + 1]);
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
	char *buf = (char*) malloc((uint32_t) fsize);
	uint32_t done = 0;
	while (done < fsize) {
		uint32_t bread = 0;
		afc_file_read(afc->client, f, buf + done, 65536, &bread);
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
