/**
  * GreenPois0n Apparition - crashreporter.c
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
#include <string.h>
#include <stddef.h>

#include <libimobiledevice/libimobiledevice.h>

#include "device.h"
#include "lockdown.h"
#include "crashreporter.h"

crashreporter_t* crashreporter_create() {
	crashreporter_t* crashreporter = (crashreporter_t*) malloc(sizeof(crashreporter_t));
	memset(crashreporter, '\0', sizeof(crashreporter_t));
	if (crashreporter == NULL) {
		//crashreportmover_free(mover);
		//crashreportcopy_free(copier);
		return NULL;
	}
	memset(crashreporter, '\0', sizeof(crashreporter_t));

	crashreporter->mover = crashreportmover_create();
	crashreporter->copier = crashreportcopy_create();
	crashreporter->afc = afc_create();
	return crashreporter;
}

void crashreporter_free(crashreporter_t* crashreporter) {
	if (crashreporter) {
		if (crashreporter->mover) {
			crashreportmover_free(crashreporter->mover);
		}
		if (crashreporter->copier) {
			crashreportcopy_free(crashreporter->copier);
		}
		free(crashreporter);
	}
}

crashreporter_t* crashreporter_open(device_t* device) {
	int err = 0;

	// Sanity check the arguments as always
	if(device == NULL || device->lockdown == NULL) {
		printf("Unable to open crashreporter service due to invalid arguments\n");
		return NULL;
	}
	lockdown_t* lockdown = device->lockdown;

	// Check and make sure our crashreporter context has already been created
	if(lockdown->crashreporter == NULL) {
		lockdown->crashreporter = crashreporter_create();
		if(lockdown->crashreporter == NULL) {
			printf("Unable to create crashreporter context\n");
			return NULL;
		}
	}
	crashreporter_t* crashreporter = lockdown->crashreporter;

	err = crashreporter_copy(crashreporter);
	if(err < 0) {
		printf("Unable to open crashreporter's copy service\n");
		return NULL;
	}
	
	err = crashreporter_move(crashreporter);
	if(err < 0) {
		printf("Unable to open crashreporter's copy service\n");
		return NULL;
	}
	/*
	afc_t* afc = afc_connect(device, port);
	if(afc == NULL) {
		printf("Unable to create afc context for crashreport\n");
		return NULL;
	}

	idevice_t device = device->client;

	afc_client_t afc = NULL;
	afc_error_t afc_error = AFC_E_SUCCESS;
	unsigned short port = 0;
	afc_error = afc_client_new(device->client, crashreporter->copier->port, &afc);
	if(afc_error != AFC_E_SUCCESS) {

		printf("afc client error: %i\n", afc_error);
		printf("Failed to create new afc client!\n");

		return NULL;
	}

	// These are here mostly for easy access. We could probably remove the lockdown stuff
	//   and always fetch it from device, but that wouldn't be very much fun, would it?
	device->lockdown->crashreporter->device = device;
	device->lockdown->crashreporter->lockdown = device->lockdown;
	*/
	return device->lockdown->crashreporter;
}

int crashreporter_close(crashreporter_t* crashreporter) {
	return -1;
}

int crashreporter_move(crashreporter_t* crashreporter) {
	// Startup crashreportmover service to move our crashes into the jailed
	//   portion of the filesystem
	crashreporter->mover = crashreportmover_open(crashreporter->device);
	if(crashreporter->mover == NULL) {
		printf("Unable to move crashreports for crashreporter\n");
		return -1;
	}
	crashreportmover_free(crashreporter->mover);
	return 0;
}

int crashreporter_copy(crashreporter_t* crashreporter) {
	// Startup crashreporter copy service to copy crashes into the jailed
	//   portion of our filesystem
	crashreporter->copier = crashreportcopy_open(crashreporter->device);
	if(crashreporter->copier == NULL) {
		printf("Unable to copy crashreports for crashreporter\n");
		return -1;
	}
	crashreportcopy_free(crashreporter->copier);
	return 0;
}



crashreport_t* crashreporter_last_crash(crashreporter_t* crashreporter) {
	uint16_t port = 0;
	afc_client_t afc = NULL;
	afc_error_t afc_error = AFC_E_SUCCESS;

	idevice_error_t device_error = IDEVICE_E_SUCCESS;

	afc_error = afc_client_new(crashreporter->device->client, crashreporter->copier->port, &afc);
	if(afc_error != AFC_E_SUCCESS) {

		printf("afc client error: %i\n", afc_error);
		printf("Failed to create new afc client!\n");

		return NULL;
	}

	char** list = NULL;
	afc_error = afc_read_directory(afc, "/", &list);
	if(afc_error != AFC_E_SUCCESS) {
		return NULL;
	}

	char *lastItem = NULL;

	int i = 0;
	int j = 0;

	time_t latest = 0;

	for(i = 0; list[i] != NULL; i++) {
		if (!(strstr(list[i], "BackupAgent2") && strstr(list[i], ".plist"))) continue;

		char **info = NULL;
		if (afc_get_file_info(afc, list[i], &info) != AFC_E_SUCCESS) continue;
		if (!info) continue;
		time_t mtime = 0;
		for (j = 0; info[j]; j += 2) {
			if (!strcmp(info[j], "st_mtime")) {
				mtime = atoll(info[j+1])/1000000000;
			}
			free(info[j]);
			free(info[j+1]);
		}
		free(info);
		if (mtime >= latest) {
			latest = mtime;
			lastItem = list[i];
		}
	}

	printf("Copying '%s'\n", lastItem);
	if (lastItem) {
		lastItem = strdup(lastItem);
	}
	for (i = 0; list[i]; i++) {
		free(list[i]);
	}
	free(list);
	if (!lastItem) {
		printf("hmm.. could not get last item\n");
		afc_client_free(afc);
		return NULL;
	}

	uint64_t handle;
	char data[0x1000];

	afc_error = afc_file_open(afc, lastItem, AFC_FOPEN_RDONLY, &handle);
	if(afc_error != AFC_E_SUCCESS) {
		printf("Unable to open %s\n", lastItem);
		free(lastItem);
		afc_client_free(afc);
		return NULL;
	}

	FILE* output = fopen(lastItem, "w");
	if(output == NULL) {
		printf("Unable to open local file %s\n", lastItem);
		free(lastItem);
		afc_file_close(afc, handle);
		afc_client_free(afc);
		return NULL;
	}

	int bytes_read = 0;
	afc_error = afc_file_read(afc, handle, data, 0x1000, &bytes_read);
	while(afc_error == AFC_E_SUCCESS && bytes_read > 0) {
		fwrite(data, 1, bytes_read, output);
		afc_error = afc_file_read(afc, handle, data, 0x1000, &bytes_read);
	}
	afc_file_close(afc, handle);
	fclose(output);

	afc_client_free(afc);

	uint32_t size = 0;
	plist_t plist = NULL;
	int err = 0;
	unsigned char* datas = NULL;
	printf("lastItem %s\n",lastItem);
	err = file_read(lastItem, &datas, &size);
	if (err < 0) {
		fprintf(stderr, "Unable to open %s\n", lastItem);
		free(lastItem);
		return NULL;
	}
	free(lastItem);
	plist_from_xml(datas, size, &plist);
	free(datas);

	return plist;
}

/*
 * crashreport_t stuff, we should probably more this to another file
 */
/*
 
 make some kind of char/struct i guess for addresses and names
 
 0x2febf000 - 0x2fee4fff  dyld armv7  <bb9bfc7d242331d29a79adf7ef7aaa18> /usr/lib/dyld
 
 we need the start address and the name (0x2febf000 and dyld respectively)
 
 
 */



dylib_info_t* crashreport_parse_dylibs(plist_t plist) {

	plist_t node = plist_dict_get_item(plist, "description");

	/* parse the output above into pointer array of aslr_magic_t structs */
	
	if (node && (plist_get_node_type(node) == PLIST_STRING)) {
		char* sval = NULL;
		char* magic = NULL;
		plist_get_string_val(node, &sval);
		
		/*
		 
		 use strstr to isolate all the text AFTER Binary Images: where the juicy stuff in the crashlog starts.
		 
		 after this we serparate the remaining objects by \n into lineArray using string tokenizing
		 
		 last but not least we cycle through lineArray and delimit those by spaces, using index 0 and 3 (start offset and binary name respectively)
		 
		 return pointer array with aslr_magic_t objects created from start offset and binary name.
		 
		 */
		
		char* magic_string = strstr(sval, "Binary Images:\n");
		int size = strlen(magic_string);
	
		char delims[] = "\n";  //lineArray delimiters
		char delims2[] = " -"; //
		char *result = NULL;
		char *result2 = NULL;
		int i = 0;
		char **lineArray = malloc(size+1);
		
		
		result = strtok(magic_string, delims );
		
		while( result != NULL ) { //separate into lines
			
			int lineSize = strlen(result);
			char *currentLine = malloc(lineSize+1);
			currentLine = result;
			lineArray[i] = currentLine;
			i++;
			result = strtok( NULL, delims );
		}
		
		printf("line count: %i\n", i);
		
		int j = 2; //we start at line two to skip over "Binary Images: and the next line which is the source program that crashed, not sure if we will need its info.
		int lineCount = i - 2; //use this in the for loop that we use to separate lineArray objects into spaceArrays for the aslr_magic_t structs
		int k = 0;
		
		dylib_info_t** finalArray = (dylib_info_t**)malloc((lineCount+1)*sizeof(dylib_info_t*)); //the big encherido.
		
		for (j = 2; j < lineCount; j++) { //separate into space delimited objects
			
			int itemIndex = 0; //reset itemIndex for currentLine
			char *currentLine = lineArray[j];
			dylib_info_t *currentMagic = malloc(sizeof(dylib_info_t));
			
			result2 = strtok(currentLine, delims2); // use " -" delimiters to divide the items by spaces.
			
			while (result2 != NULL)
			{
				
				switch (itemIndex) { //set 0 and 2 to start offset and binary name in aslr_magic_t struct
						
					case 0:
						currentMagic->startOffset = result2;
						break;
						
					case 2:
						currentMagic->binaryName = result2;
						break;
				
				}
				//printf( "item index: %i object : \"%s\"\n", itemIndex, result2 );
				
				itemIndex++;
				result2 = strtok( NULL, delims2 );
				
			}
				//printf("struct at index: %i startOffset: %s binaryName %s\n", j, currentMagic->startOffset, currentMagic->binaryName);
			finalArray[k] = currentMagic; //add current aslr_magic_t struct to the final pointer array
			k++;
		}
		finalArray[k] = NULL;
		
		free(lineArray); //FIXME: PROBABLY MORE THINGS TO RELEASE/FREE HERE!!
		
			//FIXME: below is just to show a log of the values, not actually needed.
		
		int l = 0;
		for (l = 0; l < k; l++)
		{
			dylib_info_t *currentMagic = finalArray[l];
			
			printf("dylib_info_t at index: %i startOffset: %s binaryName %s\n", l, currentMagic->startOffset, currentMagic->binaryName);
		}
	
		
		return *finalArray;
		
	}
	
	
	return NULL;
	
}

arm_state_t* crashreport_parse_state(plist_t plist) {
	return NULL;
}

void crashreport_free(crashreport_t* crash) {
	free(crash);
}
