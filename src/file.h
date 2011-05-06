/*
 * file.h
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#ifndef FILE_H_
#define FILE_H_

int file_read(const char* file, unsigned char** buf, unsigned int* length);
int file_write(const char* file, unsigned char* buf, unsigned int length);

#endif /* FILE_H_ */
