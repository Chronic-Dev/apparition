/*
 *  crashreporter.h
 *  apparition
 *
 *  Created by posixninja on 5/25/11.
 *  Copyright 2011 Chronic-Dev, LLC. All rights reserved.
 *
 */

#ifndef CRASHREPORTER_H_
#define CRASHREPORTER_H_

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/afc.h>
#include <plist/plist.h>

#include "crashreportcopy.h"
#include "afc.h"
#include "crashreportmover.h"



typedef struct aslrmagic_t
{
	char *startOffset;
	char *binaryName;
} aslrmagic_t;

typedef struct crashreporter_t {
	
	afc_t* afct;
	unsigned short port;
	lockdown_t* lockdown;
	crashreportcopy_t* copier;
	crashreportmover_t* mover;
	idevice_connection_t* connection;


} crashreporter_t;

plist_t crashreporter_last_crash(crashreporter_t* crashreporter);
crashreporter_t* crashreporter_open(lockdown_t* lockdown);
crashreporter_t* crashreporter_create(lockdown_t* lockdown);
int crashreporter_close(crashreporter_t* crashreporter);
void crashreporter_free(crashreporter_t* crashreporter);
char* magicFromDescription(plist_t node);
#endif