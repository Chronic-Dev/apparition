/*
 * mbdx_record.h
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#ifndef MBDX_RECORD_H_
#define MBDX_RECORD_H_

struct mbdx_record_t {
    unsigned char key[20];		  // key / filename
    unsigned int offset;		  // offset of the mbdb file (+ 7 bytes)
    unsigned short mode;		  // filemode: Axxx = symlink, 4xxx = dir, 8xxx = file
} __attribute__((__packed__));

typedef struct mbdx_record_t mbdx_record_t;

#endif /* MBDX_RECORD_H_ */
