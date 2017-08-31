#!/bin/bash -x

# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

source ./bannedsites.sh

WORKDIR=$PWD

array=$1
time=$2

dir=$3
zenith=$4
#ts=$5

site="PARANAL"
jobname="Sensitivity_${site}_${array}_${dir}_${time}"

if [ $dir == "N" ] || [ $dir == "S"  ]; then
  macro="sensiNS.sh"
elif [ $dir == "A" ]; then
  macro="sensiAv.sh"
fi

ROOTmacro="CTAsensi2017.C"
#ROOTmacro="CTAsensi_tweaked.C"

JDLNAME=$WORKDIR/start_${jobname}.jdl
cat > $JDLNAME <<EOF

JobName = "${jobname}";
BannedSites = {${bannedsites}};
Executable = ${macro};
Site = "LCG.IN2P3-CC.fr";
Arguments = "${array} ${site} ${time} ${dir} ${zenith}";
StdOutput = "${jobname}.out";
StdError = "${jobname}.out";
OutputSandbox = {"${jobname}.out"};
InputSandbox = {${macro}, "setupPackageMARS.sh", ${ROOTmacro}};

EOF

dirac-wms-job-submit $JDLNAME
rm -f  $JDLNAME
