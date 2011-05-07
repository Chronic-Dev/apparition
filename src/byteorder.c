/*
 * byteorder.c
 *
 *  Created on: May 7, 2011
 *      Author: posixninja
 */

#include "byteorder.h"

unsigned int flip32(unsigned int value) {
	unsigned int ret = 0;
	ret |= (value & 0xFF000000) >> 24;
	ret |= (value & 0x00FF0000) >> 8;
	ret |= (value & 0x0000FF00) << 8;
	ret |= (value & 0x000000FF) << 24;
	return ret;
}

unsigned short flip16(unsigned short value) {
	unsigned short ret = 0;
	ret |= (value & 0xFF00) >> 8;
	ret |= (value & 0x000FF) << 8;
	return ret;
}
