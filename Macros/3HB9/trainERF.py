#!/usr/bin/env python
"""
  Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

  Submit a Job to train the energy RF
    The training is run in parallel for each telescope type. It is NOT possible to hadd the output
    It should be run once with ~10-30 files, get the statistic and re-run with the number of files
    needed to reach ~150000 images in each case (~200000 for diffuse). These number are read from
    the StatFile (see that and the trainERF.sh for explanation). The job uses part of the 20% gammas
    stereo files produced using only the DirLUT (30% gamma-diffuse).
"""
from DIRAC.Core.Base import Script
Script.setUsageMessage('\n'.join([__doc__.split('\n')[1],
                       'Usage:',
                                 '  %s [option|cfgfile] ...' % Script.scriptName,
                                  'Arguments:',
                                  '  direction:    "N" or "S"',
                                  '  zenith:      "20" or "40"',
                                  '  diffuse:      "0" or "1"']))

Script.parseCommandLine()


def TrainERF(args=None):

    from DIRAC.Interfaces.API.Dirac import Dirac
    from DIRAC.Interfaces.API.Job import Job

    user_script = './trainERF.sh'

    if (len(args) != 3):
        Script.showHelp()
    direction = args[0]
    zenith = args[1]
    diffuse = args[2]

    site = "PARANAL"

    if diffuse == "0":
        diffName = "point"
    elif diffuse == "1":
        diffName = "diff"
    else:
        print "Invalid extension definition!"
        Script.showHelp()
        return 1

    if zenith == "40":
        zenName = "40deg"
    elif zenith == "20":
        zenName = "20deg"
    else:
        print "Invalid zenith definition!"
        Script.showHelp()
        return 1

    if direction == "N":
        directionName = "north"
        deg = "180"
    elif direction == "S":
        directionName = "south"
        deg = "0"
    else:
        print 'Wrong direction. It can only be "N" or "S".'
        Script.showHelp()
        return 1

    # List of files over which the training should be done
    LFN_file = './training/gamma_trainERF_%s_%s_%s.lfns' % (zenName, diffName, direction)

    StatFile = './Statistic_train.txt'
    for telType in range(0, 6):
        jobName = "%s_%s_%s_%s_%s" % (user_script, directionName, telType, diffName, zenName)
        jobOut = "%s_%s_%s.out" % (user_script, directionName, telType)
        script_args = "%s %s %s %s %s" % (direction, site, diffName, zenName, telType)
        j = Job()
        # create LFN list
        # LFNList = []
        # f = open(LFN_file, 'r')
        # for line in f:
        #    infileLFN = line.strip()
        #    LFNList.append(infileLFN)
        j.setInputSandbox([user_script, "setupPackageMARS.sh", LFN_file, StatFile])
        j.setExecutable(user_script, script_args)
        j.setOutputSandbox([jobOut, "applicationLog.txt"])
        j.setName(jobName)
        Script.gLogger.info(j._toJDL())
        print "Launching %s %s" % (user_script, script_args)
        Dirac().submit(j)

if __name__ == '__main__':

    args = Script.getPositionalArgs()

    try:
        TrainERF(args)
    except Exception:
        Script.gLogger.exception()
