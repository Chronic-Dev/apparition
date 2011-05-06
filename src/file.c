/*
 * file.c
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#include <stdio.h>

#include "file.h"

int file_read(const char* file, unsigned char** buf, unsigned int* length) {
	FILE* fd = NULL;
	fd = fopen(file, "r+");
	if(fd == NULL) {
		return -1;
	}

	fseek(fd, 0, SEEK_END);
	long size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	unsigned char* data = malloc(size);

	int bytes = fread(data, 1, size, fd);
	if(bytes != size) {
		return -1;
	}

	*buf = data;
	return bytes;
}

int file_write(const char* file, unsigned char* buf, unsigned int length) {
	return -1;
}
