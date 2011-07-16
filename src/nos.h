/**
  * GreenPois0n Apparition - nos.h
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

#ifndef NOS_H_
#define NOS_H_

#include <libimobiledevice/notification_proxy.h>

typedef np_notify_cb_t nos_cb_t;

struct device_t;
struct lockdown_t;
typedef struct nos_t {
	np_client_t client;
	struct lockdown_t* lockdown;
} nos_t;

nos_t* nos_create();
void nos_free(nos_t* nos);

nos_t* nos_open(struct device_t* device);
void nos_close(nos_t* nos);

int nos_register(nos_t* nos, nos_cb_t callback, void *arg);
void nos_perform_notification(nos_t* nos, const char *notification);

#endif /* NOS_H_ */
