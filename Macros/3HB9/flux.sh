#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

mv CheckFileZombie.C ${MARSSYS}/macros
mv CTAflux_speeed.C ${MARSSYS}/macros

particle="$1"
site="$2"
numjob="$3"
direction="$4"
#arrayflux="$5"
MOD="$5"
exten=$6
zenName=$7

if [ $direction == "N" ]; then
	directionname="north"
else
	directionname="south"
fi

if [ $particle == "gamma" ]; then
	particleName="Gamma"
elif [ $particle == "electron" ]; then
	particleName="Electron"
elif [ $particle == "proton" ]; then
	particleName="Proton"
fi

# Files to download:
stereolist=`ls | grep "lfnStereoFiles"`

# This is to decide how many files can fails the download
COUNTERN=0

while read line
do
  dirac-dms-get-file $line

  if [ $? -gt 0 ]; then
    let COUNTERN=COUNTERN+1
  fi

  if [ $COUNTERN -gt 0 ]; then
    echo "Not all input files were copied. Exiting..."
    exit 1
  fi

done <$stereolist

ls *.tar.gz | xargs -n1 tar zxf

ls | grep "stereo_Subarray" > stereoFolders

while read stereoFolder
do

  cd $stereoFolder
	cp ../.rootrc ./
	array=`echo $stereoFolder | cut -c 16-`

	# --- Remove zombie root files ---
	ls | grep ".root" > rootFileList
	while read line
	do
		# Check if the files has been made a Zombie. If yes, delete it and continue
		root.exe -l -b -q "${MARSSYS}/macros/CheckFileZombie.C+(\"${line}\")"
		if [ $? -gt 0 ]; then
			# - Finish
			echo " - Careful with the Zombie!!  D:   -->  $line"
			rm -rf $line
		fi
	done < "rootFileList"
	# --- ---
	ls
	mkdir ${particleName}

	if [ $exten == "diff" ]; then
		root.exe -l -b -q "${MARSSYS}/macros/CTAflux_speeed.C+(1,0,\"${particleName}/.\",1)"
		if [ $? -gt 0 ]; then
			# - Finish
			echo " - ROOT macro exited with error! "
			exit 2
		fi
	else
		root.exe -l -b -q "${MARSSYS}/macros/CTAflux_speeed.C+(1,0,\"${particleName}/.\")"
		if [ $? -gt 0 ]; then
			# - Finish
			echo " - ROOT macro exited with error! "
			exit 3
		fi
	fi

	rm -f rootFileList
	rm -f *.root
	rm -f .rootrc

	cd ${particleName}
	FluxFile=`ls | grep ".root"`

	mv $FluxFile CTAflux_${particle}_$numjob.root

	if [ $? -gt 0 ]; then
		# - Finish
		echo " - There's no output file!"
		exit 4
	fi

	cd ..

	cd ..
done <"stereoFolders"

outputfilename="CTAflux${particle}_${direction}_${numjob}_$MOD.tgz"

tar zcf $outputfilename stereo_Subarray*
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Could not tar..."
	exit 5
fi

dirac-dms-remove-files ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenName}/${exten}/${directionname}/flux/${outputfilename}

dirac-dms-add-file ${baseoutpath}/MC/PROD3/${site}/3HB9/${zenName}/${exten}/${directionname}/flux/${outputfilename} ${outputfilename} CC-IN2P3-USER
	if [ $? -gt 0 ]; then
		# - Finish
		echo " - Could not upload..."
		exit 6
	fi

echo " - Everything went well "
exit 0
