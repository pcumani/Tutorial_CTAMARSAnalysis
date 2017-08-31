#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!!

for array in 3HB9-ND 3HB9-LST 3HB9-MST-N; do
  for obstime in 0.5 5 50; do
    for obstype in A N S; do
      source dirac_sensi.sh ${array} ${obstime} ${obstype} 20deg
    done
  done
done
