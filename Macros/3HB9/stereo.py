#!/usr/bin/env python
"""
  Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

  Submit a Job to run CTAstereo
"""
from DIRAC.Core.Base import Script
Script.setUsageMessage('\n'.join([__doc__.split('\n')[1],
                                  'Usage:',
                                  '  %s [option|cfgfile] ...' % Script.scriptName,
                                 'Arguments:',
                                  '  particle :      "gamma", "proton" or "electron"',
                                  '  type of data :      "train" or "test"',
                                  '  direction:      "N" or "S"',
                                  '  zenith:      "20" or "40"',
                                  '  diffuse:        "0" or "1"']))

Script.parseCommandLine()


def Stereo(args=None):

    from DIRAC.Interfaces.API.Dirac import Dirac
    from DIRAC.Interfaces.API.Job import Job

    user_script = './stereo.sh'
    macro = './CTAstereo.C'

    if (len(args) != 5):
        Script.showHelp()

    particle = args[0]
    typeofdata = args[1]
    direction = args[2]
    zenith = args[3]
    diffuse = args[4]

    if typeofdata == 'train':
        # The master layout with all the telescopes
        candidates = './Prod3_3HB9All_Candidates.txt'
    elif typeofdata == 'test':
        # Different layouts
        candidates = './Prod3_3HB9_Candidates_Full.txt'
    else:
        print "Invalid type of data definition!"
        Script.showHelp()
        return 1

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

    filesPerJob = 5

    site = "PARANAL"

    listname = './training/gamma_trainLUT_%s_%s_%s.lfns' % (zenName, diffName, direction)

    loop = 0
    iJob = 0
    # totalEntries /= (2*filesPerJob)
    # print totalEntries

    f = open(listname, 'r')
    totalEntries = sum(1 for _ in f)
    f = open(listname, 'r')
    fileList = []
    text_file_name = "lfnFiles_%s_%s_%s_%s.txt" % (particle, direction, zenName, diffuse)
    text_file = open(text_file_name, "w")
    for line in f:
        loop = loop+1
        infileLFN = line.strip()
        # filein = os.path.basename(infileLFN)
        fileList.append(infileLFN)
        text_file.write("%s\n" % infileLFN)
        remain = loop % filesPerJob

        if iJob == 10:
            break

        if loop == totalEntries:
            remain = 0

        if remain == 0:
            iJob = iJob+1

            j = Job()
            text_file.close()
            j.setInputSandbox([user_script, "setupPackageMARS.sh", text_file_name, candidates, macro])
            jobName = "%s %s %s %s %s %s %s %s" % (user_script, site, particle, typeofdata, directionName, zenName, diffName, iJob)
            jobOut = "%s_%s_%s_%s_%s.out" % (user_script, site, typeofdata, directionName, iJob)
            script_args = "%s %s %s %s %s %s %s" % (particle, typeofdata, direction, zenName, diffName, site, iJob)

            j.setExecutable(user_script, script_args)
            j.setOutputSandbox([jobOut, "applicationLog.txt"])
            j.setName(jobName)
            j.setBannedSites(['LCG.MSFG.fr', 'LCG.M3PEC.fr', 'LCG.OBSPM.fr', 'LCG.UNI-DORTMUND.de', 'LCG.UNIV-LILLE.fr', 'LCG.GRIF.fr', 'ARC.SE-SNIC-T2.se'])
            Script.gLogger.info(j._toJDL())

            print "Submitting job %s" % (jobName)
            Dirac().submit(j)
            fileList = []
            text_file = open(text_file_name, "w")

if __name__ == '__main__':

    args = Script.getPositionalArgs()

    try:
        Stereo(args)
    except Exception:
        Script.gLogger.exception()
