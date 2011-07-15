/**
  * GreenPois0n Apparition - backup_file.h
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

#ifndef BACKUP_FILE_H_
#define BACKUP_FILE_H_

#include "mbdx_record.h"
#include "mbdb_record.h"

typedef struct backup_file_t {
	mbdx_record_t* mbdx_record;
	mbdb_record_t* mbdb_record;
	char *filepath;
} backup_file_t;

backup_file_t* backup_file_create(char *backupfile);

#endif /* BACKUP_FILE_H_ */
