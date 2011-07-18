/**
  * GreenPois0n Apparition - service.h
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

#ifndef SERVICE_H_
#define SERVICE_H_

struct device_t;
typedef struct service_t {
	char* name;
	uint16_t port;
	idevice_connection_t connection;

	struct service_t*(*service_create)(void);
	void(*service_free)(struct service_t* service);

	struct service_t*(*service_open)(struct device_t* device, uint16_t port);
	int service_close(struct service_t* service);

	struct service_t*(*service_start)(struct device_t* device);
	int(*service_stop)(struct service_t* service);

} service_t;

service_t* service_create();
void service_free(service_t* service);

service_t* service_open(device_t* device, uint16_t port);
int service_close(service_t* service);

service_t* service_start(device_t* device, char* service_name);
int service_stop(service_t* service);

#endif /* SERVICE_H_ */
