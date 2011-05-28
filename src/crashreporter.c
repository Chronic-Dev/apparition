/*
 *  crashreporter.c
 *  apparition
 *
 *  Created by posixninja on 5/25/11.
 *  Copyright 2011 Chronic-Dev, LLC All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "crashreporter.h"

plist_t crashreporter_last_crash(crashreporter_t* crashreporter) {
	
	lockdown_t *lockdown = crashreporter->lockdown;
	idevice_t device = lockdown->device->client;
	
	afc_error_t afc_error = AFC_E_SUCCESS;
	afc_client_t afc = NULL;
	unsigned short port = 0;
	
	idevice_error_t device_error = IDEVICE_E_SUCCESS;
	lockdownd_error_t lockdownd_error = LOCKDOWN_E_SUCCESS;
	
		//idevice_set_debug_level(3);
	
	afc_error = afc_client_new(device, crashreporter->copier->port, &afc);
	if(afc_error != AFC_E_SUCCESS) {
		
		printf("afc client error: %i\n", afc_error);
		printf("Failed to create new afc client!\n");
		
		return -1;
	}
	
	char** list = NULL;
	afc_error = afc_read_directory(afc, "/", &list);
	if(afc_error != AFC_E_SUCCESS) {
		return -1;
	}
	char *lastItem = NULL;
	
	int i = 0;
	uint64_t handle;
	char data[0x1000];
	memset(data, '\0', 0x1000);
	for(i = 0; list[i] != NULL; i++) {
		char* entry = list[i];
		lastItem = list[i];
		if(entry[0] == '.' || strstr(entry, "plist") == NULL) continue;
		printf("Copying %s index: %i\n", entry, i);
		afc_error = afc_file_open(afc, entry, AFC_FOPEN_RDONLY, &handle);
		if(afc_error != AFC_E_SUCCESS) {
			printf("Unable to open %s\n", entry);
			continue;
		}
		
		FILE* output = fopen(entry, "w");
		if(output == NULL) {
			printf("Unable to open local file %s\n", entry);
			afc_file_close(afc, handle);
			continue;
		}
		
		int bytes_read = 0;
		afc_error = afc_file_read(afc, handle, data, 0x1000, &bytes_read);
		while(afc_error == AFC_E_SUCCESS && bytes_read > 0) {
			fwrite(data, 1, bytes_read, output);
			afc_error = afc_file_read(afc, handle, data, 0x1000, &bytes_read);
		}
		afc_file_close(afc, handle);
		fclose(output);
		
	}
	
	afc_client_free(afc);
	
	
	uint32_t size = 0;
	plist_t plist = NULL;
	int err = 0;
	unsigned char* datas = NULL;
	printf("lastItem %s\n",lastItem);
	err = file_read(lastItem, &datas, &size);
	if (err < 0) {
		fprintf(stderr, "Unable to open %s\n", lastItem);
		return NULL;
	}
	plist_from_xml(datas, size, &plist);
	
	plist_t node = plist_dict_get_item(plist, "description"); //grab the plist value of the description node and return that instead of the full plist
	
	/*
	 
	 plist_t node = plist_dict_get_item(plist, "description");
	 if (node && (plist_get_node_type(node) == PLIST_STRING)) {
	 char* sval = NULL;
	 plist_get_string_val(node, &sval);
	 
	 */
	
		//printf("plist_from_xml: %s\n", plist);
	return node; 
}

/*
 
 make some kind of char/struct i guess for addresses and names
 
 0x2febf000 - 0x2fee4fff  dyld armv7  <bb9bfc7d242331d29a79adf7ef7aaa18> /usr/lib/dyld
 
 we need the start address and the name (0x2febf000 and dyld respectively)
 
 
 this is separating the description into lines properly, but still having trouble separating each line into spaces, possibly some use of strtok_r?
 
 */



char** magicFromDescription(plist_t node)
{
	/* parse the output above into some kind of a char or struct */
	
	if (node && (plist_get_node_type(node) == PLIST_STRING)) {
		char* sval = NULL;
		char* magic = NULL;
		plist_get_string_val(node, &sval);
	
		char* magic_string = strstr(sval, "Binary Images:\n");

		int size = strlen(magic_string);
		
			//printf("magic_string size: %i\n", size);
		
		
		char delims[] = "\n";
		char delims2[] = " -";
		char *result = NULL;
		char *result2 = NULL;
		char **lineArray = malloc(size+1);
		int i = 0;
		
		result = strtok(magic_string, delims );
		
		while( result != NULL ) { //separate into lines
			
			int lineSize = strlen(result);
			char *currentLine = malloc(lineSize+1);
			currentLine = result;
			lineArray[i] = currentLine;
			i++;
			result = strtok( NULL, delims );
		}
	
		printf("item count: %i\n", i);
		
		int j = 2;
		int lineCount = i - 2;
		int k = 0;
		
		char **finalArray = malloc(lineCount+1);

	
		for (j = 2; j < lineCount; j++) { //separate into space delimited objects
			
			int itemIndex = 0;
			
				//printf("lineIndex: %i\n", j);
			char *theChar = lineArray[j];
			aslrmagic_t *currentMagic = malloc(sizeof(aslrmagic_t));
			result2 = strtok(theChar, delims2);
			
			while (result2 != NULL)
			{
				if (itemIndex == 0)
				{
					currentMagic->startOffset = result2;
				} else if (itemIndex == 2)
				{
					currentMagic->binaryName = result2;
				}
					//printf( "item index: %i object : \"%s\"\n", itemIndex, result2 );
				itemIndex++;
				result2 = strtok( NULL, delims2 );
				
			}
				//printf("struct at index: %i startOffset: %s binaryName %s\n", j, currentMagic->startOffset, currentMagic->binaryName);
			finalArray[k] = currentMagic;
			k++;
		}
		
		free(lineArray); //FIXME: PROBABLY MORE THINGS TO RELEASE/FREE HERE!!
		
			//FIXME: below is just to show a log of the values, not actually needed.
		
		int l = 0;
		for (l = 0; l < k; l++)
		{
			aslrmagic_t *currentMagic = finalArray[l];
			
				printf("aslrmagic_t at index: %i startOffset: %s binaryName %s\n", l, currentMagic->startOffset, currentMagic->binaryName);
		}
		
		
		return finalArray;
		
	}
	
	
	return NULL;
	
}

	


crashreporter_t* crashreporter_open(lockdown_t* lockdown) {
	
	int err = 0;
	
		// Create our crashreport object

	crashreporter_t* crashreporter = crashreporter_create(lockdown);
	if(crashreporter == NULL) {
		return NULL;
	}
	
		// Startup crashreportmover service to move our crashes to the proper place ???
	crashreporter->mover = crashreportermover_open(lockdown);
	if(crashreporter->mover == NULL) {
		
		printf("failed to open crashreportermover_open!\n");
		
		return NULL;
	}
	
		// Startup crashreporter copy to copy them to mobile root??
	
	crashreporter->copier = crashreportcopy_open(lockdown);
	if(crashreporter->copier == NULL) {
		return NULL;
	}
	

	return crashreporter;
}



crashreporter_t* crashreporter_create(lockdown_t* lockdown) {
	crashreporter_t* crashreporter = NULL;
	
	crashreporter = (crashreporter_t*) malloc(sizeof(crashreporter_t));
	if (crashreporter == NULL) {
		return NULL;
	}
	
	memset(crashreporter, '\0', sizeof(crashreporter_t));
	lockdown->crashreporter = crashreporter;
	crashreporter->lockdown = lockdown;
	return crashreporter;
}

int crashreporter_close(crashreporter_t* crashreporter) {
	return -1;
}

void crashreporter_free(crashreporter_t* crashreporter) {
	
}