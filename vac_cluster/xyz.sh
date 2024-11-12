#!/bin/bash

for f in *; do
  if [ -d $f ]; then
    (cd $f && 'atomsk.exe' POSCAR xyz && 'atomsk.exe' POSCAR cif)
  fi
done