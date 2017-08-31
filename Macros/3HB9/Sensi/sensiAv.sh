#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

array=$1
site=$2
time=$3
zenName=$5
#ts=$6
ROOTmacro=`ls ./ | grep CTAsensi`

#mv CTAsensi_tweaked.C $MARSSYS/macros/
mv ${ROOTmacro} $MARSSYS/macros/

cd $MARSSYS

MODf="smooth"
MODeg="201612"
MOD="201612"

dirac-dms-user-lfns -w "*${MOD}.tgz" -b /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/north/flux/

#if [ $ts == "TS" ]; then
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-north-flux.lfns | grep TS > fluxFilesToDownloadN.lfns
#else
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-north-flux.lfns | grep -v TS > fluxFilesToDownloadN.lfns
#fi
cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-north-flux.lfns > fluxFilesToDownloadN.lfns

dirac-dms-get-file fluxFilesToDownloadN.lfns
if [ $? -gt 0 ]; then
    echo "Not all input files were copied. Exiting..."
    exit 2
fi

ls *.tgz | xargs -n1 tar zxf
rm *.tgz
cd "stereo_Subarray$array"
rm rootFileList_*
cd Electron
mkdir subsample1
mv *.root ./subsample1
cd ..
cd Gamma
mkdir subsample1
mv *.root ./subsample1
cd ..
cd Proton
mkdir subsample1
mv *.root ./subsample1
cd ..
cd ..

dirac-dms-user-lfns -w "*${MOD}.tgz" -b /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/south/flux/

#if [ $ts == "TS" ]; then
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-south-flux.lfns | grep TS > fluxFilesToDownloadS.lfns
#else
#	cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-south-flux.lfns | grep -v TS > fluxFilesToDownloadS.lfns
#fi
cat vo.cta.in2p3.fr-user-p-pcumani-MC-PROD3-${site}-3HB9-${zenName}-point-south-flux.lfns > fluxFilesToDownloadS.lfns

dirac-dms-get-file fluxFilesToDownloadS.lfns

if [ $? -gt 0 ]; then
    echo "Not all input files were copied. Exiting..."
    exit 2
fi
ls *.tgz | xargs -n1 tar zxf
rm *.tgz

cd "stereo_Subarray$array"
rm rootFileList_*
cd Electron
mkdir subsample2
mv *.root ./subsample2
cd ..
cd Gamma
mkdir subsample2
mv *.root ./subsample2
cd ..
cd Proton
mkdir subsample2
mv *.root ./subsample2
cd ..

cp ../../.rootrc ./

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


dirac-dms-remove-files /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/sensitivity/sensitivity_${array}_${time}_average_${MODf}.tgz
echo `ls`


dirac-dms-add-file /vo.cta.in2p3.fr/user/p/pcumani/MC/PROD3/${site}/3HB9/${zenName}/point/sensitivity/sensitivity_${array}_${time}_average_${MODf}.tgz sensitivity.tgz CC-IN2P3-USER
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not add .tgz "
	exit 1
fi
# - Finish
echo " - Everything went well "
exit 0
