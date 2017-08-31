#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

# This macro will launch the job to install Chimp in Grid, before doing so
# CHECK ALL THE FILES AND EVENTUALLY CHANGE THE PATH TO FILES, VARIABLES ETC...

source ./bannedsites.sh ;

WORKDIR=$PWD

jobname="ChimpInstall"
JDLNAME=$WORKDIR/start_${jobname}.jdl
cat > $JDLNAME <<EOF

JobName = "${jobname}";
BannedSites = {${bannedsites}};
Executable = "chimp_install.sh";
Arguments = "";
StdOutput = "${jobname}.out";
StdError = "${jobname}.out";
OutputSandbox = {"${jobname}.out"};
InputSandbox = {"chimp_install.sh", "functions_t.sh"};


EOF

dirac-wms-job-submit $JDLNAME
rm -f  $JDLNAME

exit 0
