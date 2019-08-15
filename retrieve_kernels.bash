#!/bin/bash

remote_pfx=https://naif.jpl.nasa.gov/pub/naif/INSIGHT/kernels
local_pfx=kernels

grep TOP/ ismws.tm \
| tr -d "' " \
| sed s,.TOP/,, \
| while read k ; do
    dest=$local_pfx/$k
    destdir=$(dirname $dest)
    [ -f "$dest" ] && continue || true
    mkdir -pv $destdir
    wget -nv $remote_pfx/$k -O $dest
  done
