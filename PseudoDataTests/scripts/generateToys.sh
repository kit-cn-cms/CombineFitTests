#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

#datacardDir=/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards
targetDatacard=$1 #"$datacardDir"/limits_Spring17v2p2_ttbarincl_datacard_ljets_jge6_tge4_hdecay_ttHbb_ttbb.root
numberOfToys=$2
signalStrength=$3

#pwd
combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToys -n _$((numberOfToys))toys_sig$signalStrength --expectSignal $signalStrength $targetDatacard"
echo "$combineCmd"
eval $combineCmd
