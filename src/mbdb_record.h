/**
  * GreenPois0n Apparition - mbdb_record.h
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

#ifndef MBDB_RECORD_H_
#define MBDB_RECORD_H_

struct mbdb_t;

struct mbdb_record_t {
    char* domain;
    char* path;
    char* target;	                  // absolute path
    char* datahash;	                  // SHA1 hash
    char* unknown1;
    unsigned short mode;	          // Axxx = symlink, 4xxx = dir, 8xxx = file
    unsigned int unknown2;
    unsigned int unknown3;
    unsigned int uid;
    unsigned int gid;
    unsigned int time1;
    unsigned int time2;
    unsigned int time3;
    unsigned long long length;	      // 0 if link or dir
    unsigned char flag;	              // 0 if link or dir
    unsigned char properties;	      // number of properties
} __attribute__((__packed__));

typedef struct mbdb_record_t mbdb_record_t;

mbdb_record_t* mbdb_record_create();
mbdb_record_t* mbdb_record_parse(unsigned char* data);
void mbdb_record_debug(mbdb_record_t* record);
void mbdb_record_free(mbdb_record_t* record);
int mbdb_record_build(mbdb_record_t* record, unsigned char** data, unsigned int* size);

#endif /* MBDB_RECORD_H_ */
