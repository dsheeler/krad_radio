#!/bin/bash

if [ $# -eq 0 ]; then
  paths="clients/ lib/"
else
  paths="$*"
fi

grep -irHn 'TODO\|FIXME' --include=*.h --include=*.c $paths
