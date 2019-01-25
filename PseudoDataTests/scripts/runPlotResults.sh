#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_8_1_0
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

BASEDIR=$(dirname "$0")
SCRIPT="$BASEDIR/plotResults.C"
if test "$#" -eq 4; then
  echo "$SCRIPT(\""$1"\",\""$2"\", \""$3"\", \""$4"\")"
  root -l -b -q "$SCRIPT+(\""$1"\",\""$2"\", \""$3"\", \""$4"\")"
else
if test "$#" -eq 3; then
  echo "$SCRIPT(\""$1"\",\""$2"\", "$3")"

  root -l -b -q "$SCRIPT+(\""$1"\",\""$2"\", \""$3"\")"
else

  echo "$SCRIPT(\""$1"\",\""$2"\")"

  root -l -b -q "$SCRIPT+(\""$1"\",\""$2"\")"
fi
fi
