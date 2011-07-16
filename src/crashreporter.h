/**
  * GreenPois0n Apparition - crashreporter.h
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

#ifndef CRASHREPORTER_H_
#define CRASHREPORTER_H_

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/afc.h>
#include <plist/plist.h>

#include "afc.h"
#include "crashreportcopy.h"
#include "crashreportmover.h"

struct lockdown;
typedef struct crashreport_t {
	int zero;
} crashreport_t;


typedef struct aslrmagic_t {
	char *startOffset;
	char *binaryName;
} aslrmagic_t;

typedef struct crashreporter_t {
	afc_t* afc;
	unsigned short port;
	device_t* device;
	crashreportcopy_t* copier;
	crashreportmover_t* mover;
	idevice_connection_t* connection;
	struct lockdown_t* lockdown;
} crashreporter_t;

crashreporter_t* crashreporter_create();
void crashreporter_free(crashreporter_t* crashreporter);

crashreporter_t* crashreporter_open(device_t* lockdown);
int crashreporter_close(crashreporter_t* crashreporter);

plist_t crashreporter_last_crash(crashreporter_t* crashreporter);
aslrmagic_t** magicFromDescription(plist_t node);

void crashreport_free(crashreport_t* crash);

#endif
