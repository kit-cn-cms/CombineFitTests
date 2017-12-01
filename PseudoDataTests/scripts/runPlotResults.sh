#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/skudella/CMSSW_7_4_7
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

if test "$#" -eq 3; then
  echo "plotResults.C(\""$1"\",\""$2"\", "$3")"

  root -l -b -q "plotResults.C+(\""$1"\",\""$2"\", "$3")"
else

  echo "plotResults.C(\""$1"\",\""$2"\")"

  root -l -b -q "plotResults.C+(\""$1"\",\""$2"\")"
fi
