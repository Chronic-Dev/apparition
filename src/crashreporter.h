/*
 *  crashreporter.h
 *  apparition
 *
 *  Created by Kevin Bradley on 5/25/11.
 *  Copyright 2011 Chronic-Dev, LLC. All rights reserved.
 *
 */

#ifndef CRASHREPORTER_H_
#define CRASHREPORTER_H_

#include <libimobiledevice/libimobiledevice.h>


#include "crashreportcopy.h"
#include "afc.h"
#include "crashreportmover.h"



typedef struct crashreporter_t {
	
	afc_t* afc;
	unsigned short port;
	lockdown_t* lockdown;
	crashreportcopy_t* copier;
	crashreportmover_t* mover;
	idevice_connection_t* connection;


} crashreporter_t;

crashreporter_t* crashreporter_open(lockdown_t* lockdown);
crashreporter_t* crashreporter_create(lockdown_t* lockdown);
int crashreporter_close(crashreporter_t* crashreporter);
void crashreporter_free(crashreporter_t* crashreporter);

#endif