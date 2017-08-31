#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

array=$1
site=$2
time=$3
dir=$4
zenName=$5
#ts=$6

ROOTmacro=`ls ./ | grep CTAsensi`

#mv CTAsensi_tweaked.C $MARSSYS/macros/
mv ${ROOTmacro} $MARSSYS/macros/

cd $MARSSYS

MODf="smooth"
MODeg="201612"
MOD="201612"

if [ $dir == "N" ]; then
	directionname="north"
else
	directionname="south"
fi


dirac-dms-user-lfns -w "*${MOD}.tgz" -b /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/${directionname}/flux/

#if [ $ts == "TS" ]; then
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-${directionname}-flux.lfns | grep TS > fluxFilesToDownload.lfns
#else
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-${directionname}-flux.lfns | grep -v TS > fluxFilesToDownload.lfns
#fi

cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-${directionname}-flux.lfns> fluxFilesToDownload.lfns

dirac-dms-get-file fluxFilesToDownload.lfns
if [ $? -gt 0 ]; then
    echo "Not all input files were copied. Exiting..."
    exit 2
fi
#ls *.tgz
ls *.tgz | xargs -n1 tar zxf
rm *.tgz
cd "stereo_Subarray$array"
rm rootFileList_*
#cd ..

cp ../../.rootrc ./
ls
#root.exe -l -b -q "$MARSSYS/macros/CTAsensi_tweaked.C+(${time}, \"${array}\", \"${MODf}\")"
root.exe -l -b -q "$MARSSYS/macros/${ROOTmacro}+(${time}, \"${array}\", \"${MODf}\")"

if [ $? -gt 0 ]; then
	# - Finish
	echo " - root macro didn't work "
	exit 1
fi

if [ ${ROOTmacro} == "CTAsensi2017.C" ]; then
  ls ./Performance* | grep "Subarray" | grep "root"
else
  ls | grep "Subarray" | grep "root"
fi

if [ $? -gt 0 ]; then
	# - Finish
	echo " - root macro didn't work "
	exit 1
fi

rm -rf ./Electron
rm -rf ./Gamma
rm -rf ./Proton
cd ..

tar -zcvf sensitivity.tgz stereo_Subarray$array
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not create .tgz "
	exit 1
fi


dirac-dms-remove-files /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/${directionname}/sensitivity/sensitivity_${array}_${time}_${dir}_${MODf}.tgz
echo `ls`


dirac-dms-add-file /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/${directionname}/sensitivity_${array}_${time}_${dir}_${MODf}.tgz sensitivity.tgz CC-IN2P3-USER
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not add .tgz "
	exit 1
fi
# - Finish
echo " - Everything went well "
exit 0
