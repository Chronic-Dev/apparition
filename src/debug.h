/**
  * GreenPois0n Apparition - debug.h
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

#ifndef DEBUG_H_
#define DEBUG_H_

#define APPARITION_INFO  1
#define APPARITION_DEBUG 1
#define APPARITION_ERROR 1

#ifdef APPARITION_INFO
#define info(...) if(APPARITION_INFO) printf(__VA_ARGS__)
#else
#define info(...)
#endif

#ifdef APPARITION_DEBUG
#define debug(...) if(APPARITION_DEBUG) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#else
#define debug(...)
#endif

#ifdef APPARITION_ERROR
#define error(...) if(APPARITION_ERROR) fprintf(stderr, __VA_ARGS__)
#else
#define error(...)
#endif

void print_progress_real(double progress, int flush);
void print_progress(uint64_t current, uint64_t total);


#endif /* DEBUG_H_ */
