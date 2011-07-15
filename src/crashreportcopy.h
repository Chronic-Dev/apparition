/**
  * GreenPois0n Apparition - crashreportcopy.h
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

#ifndef CRASHREPORTCOPY_H_
#define CRASHREPORTCOPY_H_

#include <plist/plist.h>
#include <libimobiledevice/libimobiledevice.h>

#include "lockdown.h"

typedef struct crashreportcopy_t {
	unsigned short port;
	idevice_connection_t connection;
} crashreportcopy_t;

crashreportcopy_t* crashreportcopy_open(lockdown_t* lockdown);
crashreportcopy_t* crashreportcopy_create(lockdown_t* lockdown);
int crashreportercopy_close(crashreportcopy_t* copier);
void crashreportercopy_free(crashreportcopy_t* copier);

#endif /* CRASHREPORTCOPY_H_ */
