/**
  * GreenPois0n Apparition - afc.h
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

#ifndef AFC_H_
#define AFC_H_

#include <libimobiledevice/afc.h>

struct device_t;
struct lockdown_t;
typedef struct afc_t {
	uint16_t port;
	afc_client_t client;
	idevice_connection_t connection;
	struct device_t* device;
	struct lockdown_t* lockdown;
} afc_t;

afc_t* afc_create();
void afc_free(afc_t* afc);

afc_t* afc_open(struct device_t* device);
int afc_close(afc_t* afc);

int afc_connect(afc_t* afc, uint16_t port);

int afc_send_file(afc_t* afc, const char* local, const char* remote);
void apparition_afc_get_file_contents(afc_t* afc, const char *filename, char **data, uint64_t *size);
#endif /* AFC_H_ */
