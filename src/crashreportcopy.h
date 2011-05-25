/*
 * crashreportcopy.h
 *
 *  Created on: May 24, 2011
 *      Author: posixninja
 */

#ifndef CRASHREPORTCOPY_H_
#define CRASHREPORTCOPY_H_

#include <libimobiledevice/libimobiledevice.h>

typedef struct crashreportcopy {
	unsigned short port;
	idevice_connection_t connection;
} crashreportcopy;

crashreportercopy_t* crashreportercopy_open(lockdown_t* lockdown);
crashreportercopy_t* crashreportercopy_create(lockdown_t* lockdown);
int crashreportercopy_close(crashreportercopy_t* copier);
void crashreportercopy_free(crashreportercopy_t* copier);

#endif /* CRASHREPORTCOPY_H_ */
