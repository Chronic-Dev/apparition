/**
  * GreenPois0n Apparition - service.c
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

#include "service.h"

service_t* service_create() {
	service_t* service = (service_t*) malloc(sizeof(service_t));
	if(service) {
		memset(service, '\0',  sizeof(service_t));
	}
	return service;
}

void service_free(service_t* service) {
	if(service) {
		free(service);
	}
}

service_t* service_open(device_t* device, uint16_t port) {
	service_t* service = NULL;

	if(device == NULL) {
		return NULL;
	}

	if(port == 0) {
		return NULL;
	}

	service = service_create();
	if(service == NULL) {
		return NULL;
	}

	return service;
}

int service_close(service_t* service) {

}

service_t* service_start(device_t* device, char* service_name) {
	int err = 0;
	service_t* service = NULL;
	lockdown_t* lockdown = NULL;

	if(device == NULL) {
		return NULL;
	}

	if(device->lockdown == NULL) {
		lockdown = lockdown_open(device);
		if(lockdown == NULL) {
			return NULL;
		}
	}
	lockdown = device->lockdown;

	err = lockdown_start_service(lockdown, service_name, &port);
	if(err != 0 || port == 0) {
		return NULL;
	}

	return service;
}

int service_stop(service_t* service) {

}
