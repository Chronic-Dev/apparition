/**
  * GreenPois0n Apparition - mbdx.h
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

#ifndef MBDX_H
#define MBDX_H

#include "mbdb.h"
#include "mbdb_record.h"

#define MBDX_MAGIC 0x6d626478

struct mbdx_record_t;

struct mbdx_header_t {
    unsigned int magic;		  // "mbdx"
    unsigned short version;
    unsigned int count;		  // count
} __attribute__((__packed__));

typedef struct mbdx_header_t mbdx_header_t;

typedef struct mbdx_t {
	struct mbdx_header_t* header;
	struct mbdx_record_t** mbdx_records;
	struct mbdb_record_t** mbdb_records;
} mbdx_t;

mbdx_t* mbdx_create();
mbdx_t* mbdx_open(unsigned char* file);
mbdx_t* mbdx_parse(unsigned char* data, unsigned int size);
void mbdx_free(mbdx_t* mbdx);
void mbdx_header_debug(mbdx_header_t* header);

#endif

//2276
//131071
