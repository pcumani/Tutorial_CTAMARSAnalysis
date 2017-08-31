#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# Macro to install Chimp in Grid
# A tarball with an uncompiled version of Chimp have to be already saved in Grid
# ALREADY containining the hessioxxx folder

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

chimpTar="Chimp201611.tar.gz"

chimpCompiled="C${chimpTar}"

export OSTYPE="linux"

. functions_t.sh

# - Check and Set environment
echo " - Setting up environment ... "
setEnvGrid

export THIS_SETUP_SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Main directory
export CTA_PROD3_PATH=$THIS_SETUP_SCRIPT_DIR


rootVersion="5-34-32"

COMPILED_MARS_FILE="CMars_CVS_NewStereo_20160614.tgz"

#Copy and extract MARS/ROOT
#===============================================================
dirac-dms-get-file ${baseoutpath}/Software/$COMPILED_MARS_FILE
if [ $? -gt 0 ]; then
	# - Finish
	echo " - lcg-cp didn't work "
	exit 1
fi
tar xzf $COMPILED_MARS_FILE


source $CTA_PROD3_PATH/root/bin/thisroot.sh

export MARSSYS=$CTA_PROD3_PATH/Mars

export LD_LIBRARY_PATH=$MARSSYS/:$LD_LIBRARY_PATH
export PATH=$MARSSYS/.:$PATH

dirac-dms-get-file "LFN:${baseoutpath}/Software/$chimpTar"
tar -xzvf $chimpTar

export WORKDIR=`pwd`
export LD_LIBRARY_PATH=$WORKDIR/Chimp:$WORKDIR/Chimp/hessioxxx/lib:$LD_LIBRARY_PATH

cd Chimp
make
if [ $? -gt 0 ]; then
    echo " - Make didn't work..."
    exit 2
fi
cd ..
tar -zcf CChimp.tgz Chimp/
if [ $? -gt 0 ]; then
    echo " - Tar file wasn't created well..."
    exit 4
fi

dirac-dms-remove-files ${baseoutpath}/Software/$chimpCompiled
dirac-dms-add-file ${baseoutpath}/Software/$chimpCompiled CChimp.tgz CC-IN2P3-USER
