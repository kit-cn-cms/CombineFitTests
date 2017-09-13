#!/bin/bash

#CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7
#export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
#source $VO_CMS_SW_DIR/cmsset_default.sh
#cd $CMSSW_BASE
#eval `scram runtime -sh`
#cd -

input="root -l -b -q -e 'gROOT->ProcessLine(\""$1"\");'"

echo "$input"

eval "$input"
