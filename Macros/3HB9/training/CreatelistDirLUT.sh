#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE QUERY!!!

# It creates the list of the CHIMP files divided by zenith/type of observation/direction to be used
# in the training of the DirLUT

for zen in 20 40; do

  cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zen} --phiP=0.0 --analysis_prog=chimp --particle=gamma --sample=training --analysis_prog_version=2016-11-10 > gamma_trainLUT_${zen}deg_point_S.lfns
  echo "gamma_trainLUT_${zen}deg_point_S.lfns: "
  less gamma_trainLUT_${zen}deg_point_S.lfns | wc -l

  cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zen} --phiP=180.0 --analysis_prog=chimp --particle=gamma --sample=training --analysis_prog_version=2016-11-10 > gamma_trainLUT_${zen}deg_point_N.lfns
  echo "gamma_trainLUT_${zen}deg_point_N.lfns: "
  less gamma_trainLUT_${zen}deg_point_N.lfns | wc -l

  cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zen} --phiP=0.0 --analysis_prog=chimp --particle=gamma-diffuse --sample=training --analysis_prog_version=2016-11-10 > gamma_trainLUT_${zen}deg_diff_S.lfns
  echo "gamma_trainLUT_${zen}deg_diff_S.lfns: "
  less gamma_trainLUT_${zen}deg_diff_S.lfns | wc -l

  cta-prod3-query --site=Paranal --array_layout=full --thetaP=${zen} --phiP=180.0 --analysis_prog=chimp --particle=gamma-diffuse --sample=training --analysis_prog_version=2016-11-10 > gamma_trainLUT_${zen}deg_diff_N.lfns
  echo "gamma_trainLUT_${zen}deg_diff_N.lfns: "
  less gamma_trainLUT_${zen}deg_diff_N.lfns | wc -l

done
