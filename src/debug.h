/*
 * debug.h
 *
 *  Created on: May 22, 2011
 *      Author: posixninja
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define APPARITION_INFO  1
#define APPARITION_DEBUG 1
#define APPARITION_ERROR 1

#ifdef APPARITION_INFO
#define INFO(...) if(APPARITION_INFO) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

#ifdef APPARITION_DEBUG
#define DEBUG(...) if(APPARITION_DEBUG) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#ifdef APPARITION_ERROR
#define ERROR(...) if(APPARITION_ERROR) fprintf(stderr, __VA_ARGS__)
#else
#define ERROR(...)
#endif



#endif /* DEBUG_H_ */
