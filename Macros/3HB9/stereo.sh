#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

mv CTAstereo.C ${MARSSYS}/macros

particle=$1
typeOfData=$2
direction=$3
zenith=$4
diffuse=$5
site=$6
iJob=$7

candidates=`ls | grep Candidates | grep txt`
fileList=`ls | grep lfnFiles`

if [ $direction == "N" ]; then
	directionname="north"
else
	directionname="south"
fi

dirLUTName=DirLUT_${zenith}_${diffuse}_${direction}.root
EnergyRFName=EnergyRF_${zenith}_${diffuse}_${direction}_final.tgz
ghRFName=RF_${zenith}_${diffuse}_${direction}_final.tgz

# Download dirLUT
dirac-dms-get-file ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenith}/${diffuse}/${directionname}/${dirLUTName}
mv ${dirLUTName} DirLUT.root

if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not copy dirLUTs. Exiting..."
	exit 1
fi

# Download ERF
dirac-dms-get-file ${baseoutpath}/MC/PROD3/PARANAL/3HB9/${zenith}/${diffuse}/${directionname}/${EnergyRFName}
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not download energy RF. Exiting..."
	if [ ${typeOfData} == "test" ]; then
		exit 2
	fi
fi
tar xzvf ${EnergyRFName}

# Download RF
dirac-dms-get-file ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenith}/${diffuse}/${directionname}/${ghRFName}
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not download RF. Exiting..."
	if [ ${typeOfData} == "test" ]; then
		exit 3
	fi
fi
tar xzvf ${ghRFName}

#copy input files
dirac-dms-get-file ${fileList}

ls

root.exe -l -b -q "${MARSSYS}/macros/CTAstereo.C+(\"*_I.root\", \"${candidates}\", 0x07, 0.8, 50., 2, kFALSE)"
if [ $? -gt 0 ]; then
	# - Finish
	echo " - CTAstereo.C macro was not happy... :( "
	exit 4
fi

filename="${particle}_${direction}_${iJob}_Full.tgz"

if [ ${typeOfData} == "train" ]; then
	ls ./stereo_All/*.root
	if [ $? -gt 0 ]; then
		# - Finish
		echo " - Nothing to tar!!! "
		exit 5
	fi
	tar zcf ${filename} ./stereo_All
	if [ $? -gt 0 ]; then
		# - Finish
		echo " - Nothing to tar!!! "
		exit 6
	fi
else
	ls ./stereo_Subarray*/*.root
	if [ $? -gt 0 ]; then
		# - Finish
		echo " - Nothing to tar!!! "
		exit 5
	fi
	tar zcf ${filename} ./stereo_Subarray*
	if [ $? -gt 0 ]; then
		# - Finish
		echo " - Nothing to tar!!! "
		exit 6
	fi
fi

dirac-dms-remove-files ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenith}/${diffuse}/${directionname}/${typeOfData}/${filename}

dirac-dms-add-file ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenith}/${diffuse}/${directionname}/${typeOfData}/${filename} ${filename} CC-IN2P3-USER

if [ $? -gt 0 ]; then
	# - Finish
	echo " - Output file not uploaded "
	exit 7
fi

exit 0
