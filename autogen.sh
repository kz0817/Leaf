#!/bin/sh

RC_LOCAL=$(pwd)/rc.local
if [ -f $RC_LOCAL ]; then
  echo Load $RC_LOCAL
  . $RC_LOCAL
fi

if [ ! -d m4 ]; then
  mkdir m4
fi

aclocal
autoreconf -i
