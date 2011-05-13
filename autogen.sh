#!/bin/sh
aclocal
autoconf
autoheader
automake -a -c
autoreconf -i
CFLAGS="-arch i386" ./configure
