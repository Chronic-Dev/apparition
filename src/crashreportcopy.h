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

struct afc_t;
struct device_t;
typedef struct crashreportcopy_t {
	uint16_t port;
	idevice_connection_t connection;
	struct afc_t* afc;
	struct device_t* device;
} crashreportcopy_t;

crashreportcopy_t* crashreportcopy_create();
void crashreportcopy_free(crashreportcopy_t* copier);

crashreportcopy_t* crashreportcopy_open(struct device_t* device);
int crashreportcopy_close(crashreportcopy_t* copier);

#endif /* CRASHREPORTCOPY_H_ */
