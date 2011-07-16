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

struct device_t;
struct lockdown_t;

typedef struct dylib_info_t {
	char *startOffset;
	char *binaryName;
} dylib_info_t;

typedef struct arm_state_t {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
} arm_state_t;

typedef struct crashreport_t {
	dylib_info_t* dylibs;
	arm_state_t* registers;
} crashreport_t;




typedef struct crashreporter_t {
	afc_t* afc;
	uint16_t port;
	crashreportcopy_t* copier;
	crashreportmover_t* mover;
	idevice_connection_t* connection;
	struct device_t* device;
	struct lockdown_t* lockdown;
} crashreporter_t;

crashreporter_t* crashreporter_create();
void crashreporter_free(crashreporter_t* crashreporter);

crashreporter_t* crashreporter_open(struct device_t* device);
int crashreporter_close(crashreporter_t* crashreporter);

int crashreporter_move(crashreporter_t* crashreporter);
int crashreporter_copy(crashreporter_t* crashreporter);

crashreport_t* crashreporter_last_crash(crashreporter_t* crashreporter);

crashreport_t* crashreport_create(plist_t node);
void crashreport_free(crashreport_t* crash);

dylib_info_t* crashreport_parse_dylibs(plist_t node);
arm_state_t* crashreport_parse_state(plist_t node);


#endif
