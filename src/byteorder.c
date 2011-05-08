/*
 * byteorder.c
 *
 *  Created on: May 7, 2011
 *      Author: posixninja
 */

#include <stdint.h>

#include "byteorder.h"

uint16_t flip16(uint16_t value) {
	uint16_t ret = 0;
	ret |= (value & 0xFF00) >> 8;
	ret |= (value & 0x000FF) << 8;
	return ret;
}

uint32_t flip32(uint32_t value) {
	uint32_t ret = 0;
	ret |= (value & 0xFF000000) >> 24;
	ret |= (value & 0x00FF0000) >> 8;
	ret |= (value & 0x0000FF00) << 8;
	ret |= (value & 0x000000FF) << 24;
	return ret;
}

uint64_t flip64(uint64_t value) {
	uint64_t ret = 0;
	ret |= (((uint64_t) value) << 56) & 0xFF00000000000000ULL;
	ret |= (((uint64_t) value) << 40) & 0x00FF000000000000ULL;
	ret |= (((uint64_t) value) << 24) & 0x0000FF0000000000ULL;
	ret |= (((uint64_t) value) << 8)  & 0x000000FF00000000ULL;
	ret |= (((uint64_t) value) >> 8)  & 0x00000000FF000000ULL;
	ret |= (((uint64_t) value) >> 24) & 0x0000000000FF0000ULL;
	ret |= (((uint64_t) value) >> 40) & 0x000000000000FF00ULL;
	ret |= (((uint64_t) value) >> 56) & 0x00000000000000FFULL;
	return ret;
}
