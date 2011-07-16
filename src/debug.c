/**
  * GreenPois0n Apparition - debug.c
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

#include "debug.h"

static void print_progress_real(double progress, int flush) {
	int i = 0;
	printf("\r[");
	for (i = 0; i < 50; i++) {
		if (i < progress / 2) {
			printf("=");
		} else {
			printf(" ");
		}
	}
	printf("] %3.0f%%", progress);

	if (flush > 0) {
		fflush(stdout);
		if (progress == 100)
			printf("\n");
	}
}

static void print_progress(uint64_t current, uint64_t total) {
	gchar *format_size = NULL;
	double progress = ((double) current / (double) total) * 100;
	if (progress < 0)
		return;

	if (progress > 100)
		progress = 100;

	print_progress_real((double) progress, 0);

	format_size = g_format_size_for_display(current);
	printf(" (%s", format_size);
	g_free(format_size);
	format_size = g_format_size_for_display(total);
	printf("/%s)     ", format_size);
	g_free(format_size);

	fflush(stdout);
	if (progress == 100)
		printf("\n");
}
