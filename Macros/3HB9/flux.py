#!/usr/bin/env python
"""
  Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

  Submit all the fluxes jobs for a particular particle/direction/zenith/type of observation
"""
from DIRAC.Core.Base import Script
Script.setUsageMessage('\n'.join([__doc__.split('\n')[1],
                                 'Usage:',
                                  '%s [option|cfgfile] ... [runMin] ...' % Script.scriptName,
                                  'Arguments:',
                                  '  particle:    "gamma" or "proton" or "electron"',
                                  '  direction:    "N" or "S"',
                                  '  MOD:    "20161011" ...',
                                  '  diffuse:      "0" or "1"',
                                  '  zenith:      "20" or "40"',
                                  ]))

Script.parseCommandLine()


def Flux(args=None):

    from DIRAC.Interfaces.API.Dirac import Dirac
    from DIRAC.Interfaces.API.Job import Job
    import time
    import os.path

    user_script = './flux.sh'
    modmacro = './CTAflux_speeed.C'
    site = "PARANAL"

    if (len(args) != 5):
        Script.showHelp()

    typeofdata = "test"
    particle = args[0]
    direction = args[1]
    MOD = args[2]
    exten = args[3]
    zenName = args[4]

    # List of files over which flux should be run

    LFN_file = "./stereofiles/lfn_%s_%s_%s_%s.lfns" % (particle, exten, zenName, direction)

    fileLength = sum(1 for line in open(LFN_file))
    f = open(LFN_file, 'r')

    if particle == "proton":
        filesPerJob = 10
    else:
        filesPerJob = 20

    fileList = []
    text_file_name = "lfnStereoFiles_%s_%s_%s_%s.txt" % (particle, exten, typeofdata, direction)
    text_file = open(text_file_name, "w")

    # File containing the id number of files already produced. The relaunch of these jobs will be skipped
    done_file_name = "./stereofiles/done/done_%s_%s_%s_%s.lfns" % (particle, exten, zenName, direction)

    if os.path.exists(done_file_name):
        done_content = [int(line.strip()) for line in open(done_file_name, 'r')]
    else:
        done_content = []

    loop = 0
    iJob = 0

    for line in f:
        loop = loop+1
        infileLFN = line.strip()

        fileList.append(infileLFN)
        text_file.write("%s\n" % infileLFN)
        remain = loop % filesPerJob

        if remain == 0 or loop == fileLength:
            iJob = iJob+1

            # Skipping of already finished jobs
            if iJob in done_content:
                text_file.close()
                fileList = []
                text_file = open(text_file_name, "w")
                continue

            else:
                j = Job()
                text_file.close()
                j.setInputSandbox([user_script, "setupPackageMARS.sh", "CheckFileZombie.C", text_file_name, modmacro])

                jobName = "%s_%s_%s_%s_%s_%s_%s" % (user_script, site, particle, direction, iJob, exten, zenName)
                jobOut = "%s_%s_%s_%s_%s.out" % (user_script, site, particle, direction, iJob)
                script_args = "%s %s %s %s %s %s %s" % (particle,  site, iJob, direction, MOD, exten, zenName)

                j.setExecutable(user_script, script_args)
                j.setOutputSandbox([jobOut, "applicationLog.txt"])
                j.setName(jobName)
                j.setBannedSites(['LCG.MSFG.fr', 'LCG.M3PEC.fr', 'LCG.OBSPM.fr', 'LCG.UNI-DORTMUND.de', 'LCG.UNIV-LILLE.fr', 'LCG.Prague.cz', 'LCG.GRIF.fr'])
                Script.gLogger.info(j._toJDL())
                print "Submitting job %s %s %s %s %s %s" % (user_script, zenName, particle, direction, site, iJob)
                time.sleep(3)
                Dirac().submit(j)
                fileList = []
                text_file = open(text_file_name, "w")

if __name__ == '__main__':

    args = Script.getPositionalArgs()

    try:
        Flux(args)
    except Exception:
        Script.gLogger.exception()
