#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# It creates the list of already processed flux files divided by particle/zenith/type of observation/direction in order
# from them to be skipped in a new call of flux.py

for zenName in 20deg 40deg; do
  for dir in north south; do
    for exten in point diff; do
      for  particle in gamma proton electron; do

        if [ $dir == "north" ]; then
		      direction="N"
	      else
		      direction="S"
	      fi

        dirac-dms-user-lfns -w "*tgz" -b /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/PARANAL/3HB9/${zenName}/${exten}/${dir}/flux/

        donename="done_${particle}_${exten}_${zenName}_${direction}.lfns"

        cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-PARANAL-3HB9-${zenName}-${exten}-${dir}-flux.lfns | grep "${particle}" | awk -F "_" '{print $3}' > ${donename}

        echo "${zenName} $particle ${exten} $dir:"
        less ${donename} | wc -l
      done
    done
  done
done

rm vo*.lfns
