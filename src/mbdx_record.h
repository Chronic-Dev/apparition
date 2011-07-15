/**
  * GreenPois0n Apparition - mbdx_record.h
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

#ifndef mbdx_record_H
#define mbdx_record_H

#include "mbdx.h"

struct mbdx_record_t {
    unsigned char key[20];  //the Key of the file, it's also the filename in the backup directory
    //It's the same key as 9.1 backups.
    unsigned int offset;     //offset of file record in .mbdb file
    //Offsets are counted from the 7th byte. So you have to add 6 to this number to get the absolute position in the file.
    unsigned short mode;     //file mode
    //Axxx  symbolic link
    //4xxx  directory
    //8xxx  regular file
    //The meaning of xxx is unknown to me, it corresponds to the Mode field in the old backup data.
}  __attribute__((__packed__));
typedef struct mbdx_record_t mbdx_record_t;

extern mbdx_t* apparition_mbdx;

mbdx_record_t* mbdx_record_create(mbdx_t* mbdx);
mbdx_record_t* mbdx_record_parse(mbdx_t* mbdx, unsigned char* data, unsigned int size);
void mbdx_record_free(mbdx_record_t* mbdx_record);
void mbdx_record_debug(mbdx_record_t* record);
int mbdx_record_build(mbdx_record_t* record, unsigned char** data, unsigned int* size);

#endif

