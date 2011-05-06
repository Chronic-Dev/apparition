/*
 * byteorder.h
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

inline unsigned int flip32(unsigned int value) {
	unsigned int ret = 0;
	ret |= (value & 0xFF000000) >> 16;
	ret |= (value & 0x00FF0000) >> 8;
	ret |= (value & 0x0000FF00) << 8;
	ret |= (value & 0x000000FF) << 16;
	return ret;
}

inline unsigned short flip16(unsigned short value) {
	unsigned short ret = 0;
	ret |= (value & 0xFF00) >> 8;
	ret |= (value & 0x000FF) << 8;
	return ret;
}

#endif /* BYTEORDER_H_ */
