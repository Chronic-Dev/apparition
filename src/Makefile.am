AM_CFLAGS =	                   \
	$(GLOBAL_CFLAGS)           \
	$(libimobiledevice_CFLAGS) \
	$(libplist_CFLAGS)

AM_LDFLAGS =                   \
	$(GLOBAL_LIBS)             \
	$(libimobiledevice_LIBS)   \
	$(libplist_LIBS)

bin_PROGRAMS = apparition

apparition_SOURCES = apparition.c device.c backup.c backup_file.c file.c lockdown.c afc.c mb2.c nos.c mbdb.c mbdb_record.c mbdx.c mbdx_record.c byteorder.c
apparition_CFLAGS = $(AM_CFLAGS)
apparition_LDFLAGS = $(AM_LDFLAGS)