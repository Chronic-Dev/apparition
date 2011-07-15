/**
  * GreenPois0n Apparition - byteorder.h
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

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <stdint.h>

inline uint16_t flip16(uint16_t value);
inline uint32_t flip32(uint32_t value);
inline uint64_t flip64(uint64_t value);

#endif /* BYTEORDER_H_ */
