#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# Macro to add replicas of the input file in several sites
# The path to the file to be replicated should be passed as an argument

filepos=$1

echo "CYF-STORM-USER"
dirac-dms-replicate-lfn ${filepos} CYF-STORM-USER
echo "DESY-ZN-USER"
dirac-dms-replicate-lfn ${filepos} DESY-ZN-USER
echo "CC-IN2P3-USER"
dirac-dms-replicate-lfn ${filepos} CC-IN2P3-USER
#echo "CNAF-USER"
#dirac-dms-replicate-lfn ${filepos} CNAF-USER
echo "CEA-USER"
dirac-dms-replicate-lfn ${filepos} CEA-USER
echo "LPNHE-USER"
dirac-dms-replicate-lfn ${filepos} LPNHE-USER
echo "LAPP-USER"
dirac-dms-replicate-lfn ${filepos} LAPP-USER
