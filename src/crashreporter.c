/*
 * crashreporter.c
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#include <plist/plist.h>

#include "debug.h"
#include "lockdown.h"
#include "crashreporter.h"
#include "crashreportcopy.h"
#include "crashreportmover.h"


plist_t crashreporter_last_crash(crashreporter_t* crashreporter) {
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
	crashreporter->mover = crashreportmover_open(lockdown);
	if(crashreporter->mover == NULL) {
		return NULL;
	}



	// replace with function from crashreportcopy.c
	int err = lockdown_start_service(lockdown, "com.apple.crashreportcopymobile", &crashreporter->port);
	if(err < 0 ) {
		return NULL;
	}

	err = device_connect(lockdown->device, crashreporter->port, crashreporter->connection);
	if(err < 0) {
		return NULL;
	}

	crashreporter->copier = crashreportcopy_open(lockdown);
	if(crashreport->copier == NULL) {
		return NULL;
	}

/*
	afc_client_t afc = NULL;
	idevice_t device = NULL;
	lockdownd_client_t lockdownd = NULL;
	afc_error_t afc_error = AFC_E_SUCCESS;
	idevice_error_t device_error = IDEVICE_E_SUCCESS;
	lockdownd_error_t lockdownd_error = LOCKDOWN_E_SUCCESS;

	idevice_set_debug_level(3);
	device_error = idevice_new(&device, NULL);
	if (device_error != IDEVICE_E_SUCCESS) {
		return -1;
	}

	lockdownd_error = lockdownd_client_new_with_handshake(device, &lockdownd, "idevicecrashreport");
	if (lockdownd_error != LOCKDOWN_E_SUCCESS) {
		idevice_free(device);
		return -1;
	}

	unsigned short port = 0;

	lockdownd_error = lockdownd_start_service(lockdownd, "com.apple.crashreportmover", &port);
	if (lockdownd_error != LOCKDOWN_E_SUCCESS) {
		lockdownd_client_free(lockdownd);
		idevice_free(device);
		return -1;
	}

	idevice_connection_t connection = NULL;
	device_error = idevice_connect(device, port, &connection);
	if(device_error != IDEVICE_E_SUCCESS) {
		lockdownd_client_free(lockdownd);
		idevice_free(device);
		return -1;
	}
	idevice_disconnect(connection);

	lockdownd_error = lockdownd_start_service(lockdownd, "com.apple.crashreportcopymobile", &port);
	if (lockdownd_error != LOCKDOWN_E_SUCCESS) {
		lockdownd_client_free(lockdownd);
		idevice_free(device);
		return -1;
	}
	lockdownd_client_free(lockdownd);

	afc = NULL;
	afc_error = afc_client_new(device, port, &afc);
	if(afc_error != AFC_E_SUCCESS) {
		lockdownd_client_free(lockdownd);
		idevice_free(device);
		return -1;
	}

	char** list = NULL;
	//afc_error = afc_read_directory(afc, "/Baseband", &list);
	afc_error = afc_read_directory(afc, "/", &list);
	if(afc_error != AFC_E_SUCCESS) {
		afc_client_free(afc);
		idevice_free(device);
		return -1;
	}

	int i = 0;
	uint64_t handle;
	char data[0x1000];
	memset(data, '\0', 0x1000);
	for(i = 0; list[i] != NULL; i++) {
		char* entry = list[i];
		if(entry[0] == '.' || strstr(entry, "plist") == NULL) continue;
		printf("Copying %s\n", entry);
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
	idevice_free(device);
	return 0;
	*/
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
	return crashreporter;
	return NULL;
}

int crashreporter_close(crashreporter_t* crashreporter) {
	return -1;
}

void crashreporter_free(crashreporter_t* crashreporter) {
}
