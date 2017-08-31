#!/bin/bash

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# ATTENTION: VALUES TO CHANGE!!! CHANGE ALSO THE PATH TO THE DIFFERENT FILES!!!

baseoutpath="/vo.cta.in2p3.fr/user/p/pcumani"

# Setup file to configure Mars package
export USER=pcumani
export THIS_SETUP_SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Main directory
export CTA_PROD3_PATH=$THIS_SETUP_SCRIPT_DIR


rootVersion="5-34-32"

COMPILED_MARS_FILE="CMars_CVS_20161110.tgz"

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
export OSTYPE=$OSTYPE

export LD_LIBRARY_PATH=$MARSSYS/:$LD_LIBRARY_PATH
export PATH=$MARSSYS/.:$PATH
