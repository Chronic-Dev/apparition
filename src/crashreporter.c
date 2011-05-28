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
		
		return NULL;
	}
	
	char** list = NULL;
	afc_error = afc_read_directory(afc, "/", &list);
	if(afc_error != AFC_E_SUCCESS) {
		return NULL;
	}
	char *lastItem = NULL;
	
	int i = 0;
	uint64_t handle;
	char data[0x1000];
	memset(data, '\0', 0x1000);
	for(i = 0; list[i] != NULL; i++) {
		char* entry = list[i];
		
	
		if(entry[0] == '.' || strstr(entry, "plist") == NULL) continue;
		printf("Copying %s index: %i\n", entry, i);
		if (lastItem == NULL)
		{
			printf("setting last item to: %i!!\n", i);
			lastItem = list[i];
		}
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
 
 
 */



char** magicFromDescription(plist_t node) {
	
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
		
		char **finalArray = malloc(lineCount+1); //the big encherido.
		
		for (j = 2; j < lineCount; j++) { //separate into space delimited objects
			
			int itemIndex = 0; //reset itemIndex for currentLine
			char *currentLine = lineArray[j];
			aslrmagic_t *currentMagic = malloc(sizeof(aslrmagic_t));
			
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
	
	// Startup crashreportmover service to move our crashes to the proper place ???
	crashreportmover_t* mover = crashreportermover_open(lockdown);
	if(mover == NULL) {
		
		printf("failed to open crashreportermover_open!\n");
		
		return NULL;
	}
	
	// Startup crashreporter copy to copy them to mobile root??	
	crashreportcopy_t* copier = crashreportcopy_open(lockdown);
	if(copier == NULL) {
		//crashreportmover_free(mover);
		return NULL;
	}
	
	crashreporter_t* crashreporter = (crashreporter_t*) malloc(sizeof(crashreporter_t));
	memset(crashreporter, '\0', sizeof(crashreporter_t));
	if (crashreporter == NULL) {
		//crashreportmover_free(mover);
		//crashreportcopy_free(copier);
		return NULL;
	}
	memset(crashreporter, '\0', sizeof(crashreporter_t));
	crashreporter->mover = mover;
	crashreporter->copier = copier;
	crashreporter->lockdown = lockdown;

	return crashreporter;
}

int crashreporter_close(crashreporter_t* crashreporter) {
	return -1;
}

void crashreporter_free(crashreporter_t* crashreporter) {
	
}
