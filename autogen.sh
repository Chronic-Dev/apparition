#!/bin/sh
aclocal
autoconf
autoheader
automake -a -c
autoreconf -i
CFLAG="-arch i386" ./configure
