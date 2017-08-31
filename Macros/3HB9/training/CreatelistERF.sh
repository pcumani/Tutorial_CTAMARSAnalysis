#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!!

# It creates the list of the stereo files divided by zenith/type of observation/direction to be used
# in the training of the Energy RF

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

dirac-dms-user-lfns -w "*_onlyDirLUT.tgz" -b ${baseoutpath}/MC/PROD3/PARANAL/3HB9/

for zen in 20deg 40deg; do
  for exten in point diff; do
    for dir in N S; do
      #the first command is to sort the files following the numbers ( 1 2 3 instead of 100 10 1 200 20 2 300 30 3 etc)
      sort -t _ -k 3 -g vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-PARANAL-3HB9.lfns | grep _${dir}_ | grep ${zen} | grep ${exten} > gamma_trainERF_${zen}_${exten}_${dir}.lfns
      echo "${zen} ${exten} $dir:"
      less gamma_trainERF_${zen}_${exten}_${dir}.lfns | wc -l
    done
  done
done
