/**
  * GreenPois0n Apparition - crashreportmover.h
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

#ifndef CRASHREPORTMOVER_H_
#define CRASHREPORTMOVER_H_

#include <libimobiledevice/libimobiledevice.h>

struct device_t;
struct lockdown_t;
typedef struct crashreportmover_t {
	uint16_t port;
	idevice_connection_t connection;
	struct device_t* device;
	struct lockdown_t* lockdown;
} crashreportmover_t;

crashreportmover_t* crashreportmover_create();
void crashreportmover_free(crashreportmover_t* mover);

crashreportmover_t* crashreportmover_open(struct device_t* device);
int crashreportmover_close(crashreportmover_t* mover);

#endif /* CRASHREPORTMOVER_H_ */
