/**
 * GreenPois0n Apparition - apparition.h
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

#include "afc.h"
#include "mb2.h"
#include "nos.h"
#include "backup.h"
#include "device.h"
#include "lockdown.h"
#include "apparition.h"
#include "mbdx_record.h"
#include "mbdb_record.h"
#include "crashreporter.h"

// names of potential functions in this file?

int exploit_mobilebackup2(device_t* device, crashreport_t* crashreport) {
	printf(">> %s called\n", __func__);
	return -1;
}

crashreport_t* fetch_crashreport(device_t* device) {
	printf(">> %s called\n", __func__);
	int err = 0;

	// Startup mobilebackup2 service so we can crash it
	mb2_t* mb2 = mb2_open(device);
	if (mb2 == NULL) {
		printf("Unable to open connection to mobilebackup2 service");
		return NULL;
	}

	// Burn baby, burn
	err = mb2_crash(mb2);
	if (err == 0xDEAD) {
		printf("crashed!!! (hopefully)\n");
	} else {
		printf("hmmm... %d\n", err);
	}
	// We might just want to leak this for the hell of it
	//mb2_free(mb2);

	printf("Giving the device a moment to write the crash report...\n");
	sleep(3);

	// Here we open crash reporter so we can download the mobilebackup2 crash report
	//  and parse the "random" dylib addresses. Thank you ASLR for nothing :-P
	printf("Opening connection to crashreporter\n");
	crashreporter_t* reporter = crashreporter_open(device);
	if (reporter == NULL) {
		printf("Unable to open connection to crash reporter\n");
		return NULL;
	}

	// Read in the last crash since that's probably our fault anyways :-P
	printf("Reading in most recent crashreport from device\n");
	crashreport_t* crash = crashreporter_last_crash(reporter);
	if (crash == NULL) {
		printf("Unable to read last crash\n");
		return NULL;
	}
	crashreporter_close(reporter);

	return crash;
}

static void notify_cb(const char *notification, void *userdata) {
	printf(">> %s called\n", __func__);
	if (!strcmp(notification, NP_SYNC_CANCEL_REQUEST)) {
		printf("User has cancelled the backup process on the device.\n");
	} else {
		printf("Unhandled notification '%s' (TODO: implement)\n", notification);
	}
}

int main(int argc, char* argv[]) {
	device_t* device = NULL;
	crashreport_t* crashreport = NULL;
	crashreporter_t* crashreporter = NULL;

	// Pass a UUID here if you want to target a single device,
	//  or NULL to select the first one it finds
	printf("Opening device connection\n");
	device = device_open(NULL);
	if (device == NULL) {
		printf("Unable to find a device to use\n");
		goto done;
	}
	device_enable_debug(3);

	// Here we open crash reporter so we can download the mobilebackup2 crashreport
	//  and parse the "random" dylib addresses. Thank you ASLR for nothing :-P
	printf("Opening connection to crashreporter\n");
	crashreporter = crashreporter_start(device);
	if (crashreporter == NULL) {
		printf("Unable to open connection to crash reporter\n");
		goto done;
	}

	// Grab the last crash that occurred for analysis
	crashreport = crashreporter_last_crash(crashreporter);
	if(crashreport == NULL) {
		printf("Unable to fetch last crash from crashreporter service\n");
		goto done;
	}

done:
	// Always clean up our mess when we're done
	printf("Cleaning up\n");
	if(crashreporter) crashreporter_free(crashreporter);
	if(crashreport) crashreport_free(crashreport);
	if(device) device_free(device);
	printf("Done\n");
	return 0;
}
