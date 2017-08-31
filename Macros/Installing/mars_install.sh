#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# Macro to install MARS in Grid, in a single tarball with ROOT and the .rootrc
# A tarball with an uncompiled version of Mars have to be already saved in Grid

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!
MarsTar="Mars_CVS_20161110.tgz"

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

rootVersion="5-34-32"

rootCompiled="Cgnuroot_v${rootVersion}.tgz"

COMPILED_MARS_FILE="C${MarsTar}"
RAW_MARS_FILE="${baseoutpath}/Software/${MarsTar}"
export USER="pcumani"


export OSTYPE="linux-gnu"

. functions_t.sh

# - Check and Set environment
echo " - Setting up environment ... "
setEnvGrid

# Copy and "Install" root
dirac-dms-get-file "${baseoutpath}/Software/$rootCompiled"
if [ $? -gt 0 ]; then
	# - Finish
	echo " - ROOT not downloaded "
	exit 1
fi
tar -xzf $rootCompiled

# Copy .rootrc, pointing to $MARSSYS/lib and $MARSSYS/macros
dirac-dms-get-file /vo.cta.in2p3.fr/user/t/thassan/Software/.rootrc
if [ $? -gt 0 ]; then
	# - Finish
	echo " - lcg-cp didn't work "
	exit 1
fi

export WORKDIR=`pwd`
export ROOTPATH=$WORKDIR/root

export LCG_CATALOG_TYPE=lfc
export LFC_HOST=lfc-egee.in2p3.fr
export MARSSYS=$WORKDIR/Mars

source $ROOTPATH/bin/thisroot.sh

# Copy and "Install" MARS

dirac-dms-get-file "$RAW_MARS_FILE"
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Mars.tgz could not be downloaded. D: "
	exit 2
fi


echo "Untaring..."
tar -xzf ${MarsTar}
echo "...done."

mv Mars_CVS_20161110/ Mars/
cd Mars/

find ./ -type f -exec sed -i 's/kERROR/kERRORR/g' {} \;

make
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Mars didn\'t compile well (I can\'t believe it!) "
	exit 1
fi

find . -type f -name '*.cxx' -delete
find . -type f -name '*.cc' -delete


cd ..
echo "taring..."
tar zcf $COMPILED_MARS_FILE Mars/ root/ ./.rootrc
echo "...done."

echo "Deleting previous tar file..."
dirac-dms-remove-files ${baseoutpath}/Software/$COMPILED_MARS_FILE
echo "...done."

echo "Saving tar file..."
dirac-dms-add-file ${baseoutpath}/Software/$COMPILED_MARS_FILE $COMPILED_MARS_FILE CC-IN2P3-USER
if [ $? -gt 0 ]; then
	# - Finish
	echo " - Compiled Mars package could not be uploaded. What a waste of time... -.-'"
	exit 1
fi
echo "...done."
#Only if the installation was successful it deletes the tarball of uncompiled MARS
dirac-dms-remove-files "$RAW_MARS_FILE"
