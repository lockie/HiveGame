#!/bin/sh

rm -f config.cache
rm -drf autom4te.cache
libtoolize --force
echo "Running aclocal"
aclocal -I .
echo "Running autoheader"
autoheader
echo "Running automake"
automake --foreign --add-missing --include-deps &&
echo "Running autoconf"
autoconf
echo "Now you are ready to run ./configure"

exit 0
