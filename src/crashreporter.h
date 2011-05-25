/*
 * crashreporter.h
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#ifndef CRASHREPORTER_H_
#define CRASHREPORTER_H_

#include <plist/plist.h>

#include "afc.h"
#include "device.h"
#include "lockdown.h"
#include "crashreportcopy.h"
#include "crashreportmover.h"

typedef struct crashreporter_t {
	afc_t* afc;
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

#endif /* CRASHREPORTER_H_ */
