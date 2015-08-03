#!/bin/sh

set -e

autoreconf -f -i

if [ x$NOCONFIGURE = x ]; then
 ./configure "$@"
fi
