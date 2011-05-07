#!/bin/sh
aclocal
autoconf
autoheader
automake -a -c
autoreconf -i
./configure
#CFLAGS+="-g -arch i386" ./configure --prefix=/usr/local/
