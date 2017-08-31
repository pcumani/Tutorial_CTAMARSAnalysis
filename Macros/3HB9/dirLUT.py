#!/usr/bin/env python
"""
  Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

  Submit a Job to train the dirLUT
    For the DirLUT it is possible to parallelize and then hadd the results. In the same it trains all telescope
    types. The job uses the full 20% CHIMP gammas sample produced by Luisa (30% gamma-diffuse).
    The individual files created by each job should then be downloaded and hadd to have the final DirLUT
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


def dirLUT(args=None):

    from DIRAC.Interfaces.API.Dirac import Dirac
    from DIRAC.Interfaces.API.Job import Job

    user_script = './dirLUT.sh'

    if (len(args) != 3):
        Script.showHelp()
    direction = args[0]
    zenith = args[1]
    diffuse = args[2]

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
        # deg = "180"
    elif direction == "S":
        directionName = "south"
        # deg = "0"
    else:
        print 'Wrong direction. It can only be "N" or "S".'
        Script.showHelp()
        return 1

    listname = './training/gamma_trainLUT_%s_%s_%s.lfns' % (zenName, diffName, direction)

    with open(listname) as f:
        totalEntries = sum(1 for _ in f)

    # Number of files used per job
    runN = 20

    runMin = 0
    runMax = totalEntries / runN

    for i in range(runMin, runMax):
        jobName = "%s_%s_%s_%s_%s" % (user_script, direction, zenName, diffName, i)
        jobOut = "%s_%s_%s%s.out" % (user_script, directionName, diffName, i)
        script_args = "%s %s %s %s %s" % (direction, zenName, diffName, i, runN)
        j = Job()
        j.setInputSandbox([user_script, listname, "setupPackageMARS.sh", "CheckFileZombie.C"])
        j.setExecutable(user_script, script_args)
        j.setOutputSandbox([jobOut, "applicationLog.txt"])
        j.setName(jobName)
        j.setBannedSites(['LCG.MSFG.fr', 'LCG.M3PEC.fr', 'LCG.OBSPM.fr', 'LCG.UNI-DORTMUND.de', 'LCG.UNIV-LILLE.fr'])
        Script.gLogger.info(j._toJDL())
        print "Submitting job %s" % (script_args)
        Dirac().submit(j)

if __name__ == '__main__':

    args = Script.getPositionalArgs()

    try:
        dirLUT(args)
    except Exception:
        Script.gLogger.exception()
