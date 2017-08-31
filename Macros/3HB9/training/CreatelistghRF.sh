#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE QUERY!!!

# It creates the list of the stereo files created with DirLUT and ERF and divided by zenith/type of
# observation/direction to be used in the training of the gamma/hadron RF

for zen in 20 40; do
  for dir in N S; do
    for particle in gamma gamma-diffuse proton; do
      for exten in point diff; do

        if [ $particle == "gamma-diffuse" ]; then
      	   particleName="gamma"
           if [ $exten == "point" ]; then
             continue
           fi
        elif [ $particle == "proton" ]; then
          particleName=$particle
        elif [ $particle == "gamma" ]; then
      	    particleName=$particle
            if [ $exten == "diff" ]; then
              continue
            fi
        fi

        if [ $dir == "N" ]; then
      	   phi="180.0"
         else
      	    phi="0.0"
        fi

        option1="--site=Paranal --array_layout=full"
        option2="--sample=background_training --analysis_prog_version=2016-11-10-p1"
        filename="${particleName}_ghtrain_${zen}deg_${exten}_${dir}.lfns"

        cta-prod3-query ${option1} --thetaP=${zen} --phiP=${phi} --analysis_prog=ctastereo --particle=${particle} ${option2} | grep ${exten} > ${filename}
        echo "${filename}: "
        less ${filename} | wc -l
      done
    done
  done
done
