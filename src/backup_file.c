/**
  * GreenPois0n Apparition - backup_file.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "backup_file.h"
#include "mbdx_record.h"
#include "mbdb_record.h"


backup_file_t* backup_file_create(char *backupfile) {
	backup_file_t* file = (backup_file_t*) malloc(sizeof(backup_file_t));
	if(file == NULL) {
		fprintf(stderr, "Allocation Error\n");
		return NULL;
	}
	memset(file, '\0', sizeof(backup_file_t));
	file->filepath = backupfile;
	return file;
}

void backup_file_free(backup_file_t* file) {
	if(file) {
		free(file);
	}
}
