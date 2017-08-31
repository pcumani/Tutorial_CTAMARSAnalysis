#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# Macro to install ROOT in Grid
# A tarball with an uncompiled version of ROOT have to be already saved in Grid

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!
rootVersion="5-34-32"
compiledname="Cgnuroot_v${rootVersion}.tgz"

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

export OSTYPE="linux-gnu"


# Define install path
localPATH=`pwd`

. functions_t.sh

# - Check and Set environment
echo " - Setting up environment ... "
setEnvGrid

dirac-dms-get-file "${baseoutpath}/Software/root_v${rootVersion}.tgz"
if [ $? -gt 0 ]; then
    echo 'Raw root package is not in path. Exiting...'
    exit 1
fi
tar -xzvf root_v${rootVersion}.tgz
mv root/ root_v${rootVersion}


installPATH="$localPATH/root"

mkdir -p $installPATH
cd root_v${rootVersion}

export ROOTSYS=$installPATH
./configure --enable-explicitlink --enable-soversion --disable-x11 --enable-minuit2
make
if [ $? -gt 0 ]; then
    echo 'root "make" didnt not work correctly'
    exit 1
fi
make install

mv .rootrc $localPATH/

cd $localPATH
echo "taring..."
tar zcf $compiledname root/ .rootrc
echo "...done."

echo "Deleting previous tar file..."
dirac-dms-remove-files ${baseoutpath}/Software/$compiledname
echo "...done."

echo "Saving tar file..."
dirac-dms-add-file ${baseoutpath}/Software/$compiledname $compiledname DESY-ZN-USER
if [ $? -gt 0 ]; then
    echo "Could not copy data. Exiting..."
    exit 1
fi
echo "...done."


echo "Everything went well!"
exit 0
