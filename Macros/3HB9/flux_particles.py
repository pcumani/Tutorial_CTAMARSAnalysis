#!/usr/bin/env python
"""
  Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

  Submit all the Flux jobs calling flux.py
    It simply contain a loop that calls flux over all the combinations direction/particle
"""
from DIRAC.Core.Base import Script
Script.setUsageMessage('\n'.join([__doc__.split('\n')[1],
                                  'Usage:',
                                  ' %s [option|cfgfile] ... [runMin] ...' % Script.scriptName,
                                  'Arguments:',
                                  '  MOD:    "20161011" ...',
                                  '  zenith:      "20" or "40"',
                                  '  diffuse:      "0" or "1"']))

Script.parseCommandLine()


def Fluxes_particles(args=None):

    import time
    from flux import Flux

    if (len(args) != 3):
        Script.showHelp()

    # direction = args[0]
    MOD = args[0]
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

    directions = ["N", "S"]
    # directions = ["S"]
    # particles = ["gamma", "proton", "electron"]
    particles = ["gamma", "electron"]
    # particles = ["proton"]
    # particles = ["gamma"]

    for particle in particles:
        for direction in directions:
            print "Flux for %s, direction = %s, %s, extension = %s, MOD = %s" % (particle, direction, zenName, diffName, MOD)
            Flux([particle, direction, MOD, diffName, zenName])
            time.sleep(60)
            print "FINISHED FIRST LOOP!!!"

    print "FINALLY OVER!!!!"

if __name__ == '__main__':

    args = Script.getPositionalArgs()

    try:
        Fluxes_particles(args)
    except Exception:
        Script.gLogger.exception()
