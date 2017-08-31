#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

# - Check and Set environment
echo " - Setting up environment ... "
source setupPackageMARS.sh

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

mv CheckFileZombie.C $MARSSYS/macros

direction=$1
zenith=$2
diffuse=$3
numjob=$4
step=$5

if [ $direction == "N" ]; then
	directionname="north"
	deg="0"
else
	directionname="south"
	deg="180"
fi


LFN_files=`ls ./ | grep .lfns`

num_line=0

# Only step files per job
initialjob=$(($numjob*$step))
limitjob=$(($initialjob+$step))

while read line
do
  if [ $num_line -ge $initialjob ] && [ $num_line -lt $limitjob ]; then
		dirac-dms-get-file $line
		if [ $? -gt 0 ]; then
			# - Finish
			echo " - Could not copy input files."
			exit 1
		fi

		# Check if the file is a tarball or a root file. In the first case untar it (due to files produced by Konrad)
		if [ ${line##*.} != "root" ]; then
			tar zxf $line
		fi

		rootFilename=$(basename "${line%.*}.root")

		# Check if the files has been made a Zombie. If yes, delete it and continue
		root.exe -l -b -q "${MARSSYS}/macros/CheckFileZombie.C+(\"${rootFilename}\")"
		if [ $? -gt 0 ]; then
			# - Finish
			echo " - Careful with the Zombie!!  D:   -->  $rootFilename"
			rm -rf $rootFilename
		fi
  fi
    let num_line=num_line+1
done < ${LFN_files}

ls *_I.root
if [ $? -gt 0 ]; then
	# - Finish
	echo " - No input files.... :( "
	exit 2
fi

root.exe -l -b -q "${MARSSYS}/macros/CTADirlookuptable.C+(\"*_I.root\", 2, kFALSE)"
if [ $? -gt 0 ]; then
	# - Finish
	echo " - CTADirlookuptable.C macro was not happy... :( "
	exit 3
fi

mv DirLUT.root DirLUT_${direction}_${numjob}.root
if [ $? -gt 0 ]; then
	# - Finish
	echo " - CTADirlookuptable.C didn't create any file... D: "
	exit 4
fi

outputfilename="DirLUT_${zenith}_${diffuse}_${direction}_${numjob}.root"

dirac-dms-remove-files ${baseoutpath}/MC/PROD3/PARANAL/3HB9/${zenith}/${diffuse}/${directionname}/$outputfilename

dirac-dms-add-file ${baseoutpath}/MC/PROD3/PARANAL/3HB9/${zenith}/${diffuse}/${directionname}/$outputfilename DirLUT_${direction}_${numjob}.root CC-IN2P3-USER
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Output file not uploaded "
	exit 5
fi
exit 0
