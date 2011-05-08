/*
 * byteorder.h
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <stdint.h>

inline uint16_t flip16(uint16_t value);
inline uint32_t flip32(uint32_t value);
inline uint64_t flip64(uint64_t value);

#endif /* BYTEORDER_H_ */
