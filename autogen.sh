#!/bin/sh
aclocal
autoconf
automake -a -c
autoreconf -i
./configure
