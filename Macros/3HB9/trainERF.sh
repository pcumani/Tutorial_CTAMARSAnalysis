#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

direction=$1
site=$2
#scaling=$3
diffuse=$3
zenName=$4
telType=$5
lfnFiles=`ls ./ | grep "gamma_trainERF_"`
StatFile=`ls ./ | grep "Statistic_train.txt"`

if [ $direction == "N" ]; then
	directionname="north"
	deg="180"
else
	directionname="south"
	deg="0"
fi

# Download only a maxjobs number of files, maxjobs taken from the StatFile
maxjobs=`cat ${StatFile} | grep "E_${telType}_${direction}_${zenName}_${diffuse}" | awk -F "_" '{print $6}'`

cat $lfnFiles | head -n $maxjobs >FilestoDown.lfns

dirac-dms-get-file FilestoDown.lfns
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Not all files were downloaded  "
	exit 1
fi

ls *.tgz | xargs -n1 tar xzf

mv ./stereo_All/*_S.root ./

root.exe -l -b -q "${MARSSYS}/macros/CTAtrainEnergyRF.C+(\"*_S.root\", \"\", 2, kFALSE, kFALSE, $telType, $telType)"
if [ $? -gt 0 ]; then
	# - Finish
	echo " - CTADirlookuptable.C macro was not happy... :( "
	exit 2
fi

ls

filename="EnergyRF_${zenName}_${diffuse}_files_${direction}_${telType}.tgz"

tar zcf ${filename}  RF*.root

if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not create tar file "
	exit 3
fi


dirac-dms-remove-files ${baseoutpath}/MC/PROD3/PARANAL/3HB9/${zenName}/${diffuse}/${directionname}/$filename

dirac-dms-add-file ${baseoutpath}/MC/PROD3/PARANAL/3HB9/${zenName}/${diffuse}/${directionname}/$filename $filename CC-IN2P3-USER
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not add the file "
	exit 4
fi

exit 0
