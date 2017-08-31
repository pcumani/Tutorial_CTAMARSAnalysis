# Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo
echo "Reading functions_t.sh..."


setEnvGrid () {
    # - general thingy to avoid permission problems w/generic users
    umask 0000

    # - dCache
    export LCG_GFAL_INFOSYS=topbdii.core.ibergrid.eu:2170,bdii.pic.es:2170,lcg-bdii.cern.ch:2170
    export LFC_HOST=lfc-egee.in2p3.fr
    export DCACHE_RAHEAD=TRUE
    export DCACHE_RA_BUFFER=128000
    export SRM_PATH="" # to avoid warning messages
    export VO=vo.cta.in2p3.fr

    # - Data protocols
    NFSEndPoint="file:///"
    LFNEndPoint="lfn:"
    SRMEndPoint="srm://srm.ciemat.es:8443"

    # - Data directories
    LFCDataDir="/grid/$VO"
    LFCUsersDir="/grid/$VO"
}
