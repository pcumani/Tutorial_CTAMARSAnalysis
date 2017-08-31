#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# It creates the list of the stereo files divided by particle/zenith/type of observation/direction to be used
# in the creation of the flux files

for  particle in gamma gamma-diffuse proton electron; do
  for zenith in 20 40; do
    for dir in N S; do
      namepoint="lfn_${particle}_point_${zenith}deg_${dir}.lfns"
      namediff="lfn_${particle}_diff_${zenith}deg_${dir}.lfns"
      if [ $dir == "N" ]; then
        phi="180.0"
      elif [ $dir == "S" ]; then
        phi="0.0"
      fi

      if [[ $particle = "gamma-diffuse" ]]; then
        namediff="lfn_gamma_diff_${zenith}deg_${dir}.lfns"
        cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zenith} --phiP=${phi} --analysis_prog=ctastereo --particle=${particle} --sample=analysis --analysis_prog_version=2016-11-10 | grep _${dir}_ | grep diff > ${namediff}
      elif [ $particle = "gamma" ]; then
        cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zenith} --phiP=${phi} --analysis_prog=ctastereo --particle=${particle} --sample=analysis --analysis_prog_version=2016-11-10 | grep _${dir}_ | grep -v diff > ${namepoint}
      else
        cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zenith} --phiP=${phi} --analysis_prog=ctastereo --particle=${particle} --sample=analysis --analysis_prog_version=2016-11-10 | grep _${dir}_ | grep -v diff > ${namepoint}
        cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zenith} --phiP=${phi} --analysis_prog=ctastereo --particle=${particle} --sample=analysis --analysis_prog_version=2016-11-10 | grep _${dir}_ | grep diff > ${namediff}
      fi

      echo "$zenith $particle  $dir:"
      less ${namepoint} | wc -l
      less ${namediff} | wc -l
    done
  done
done
