#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

targetDatacard=$1
toyFile=$2
signalStrength=$3
numberOfToys=$4

combineCmd="combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-5.00 --rMax=5.00 -t $numberOfToys --toysFile $toyFile --minos all -n _sig$((signalStrength))_$((numberOfToys))toys $targetDatacard"
echo "$combineCmd"
eval $combineCmd
#rm *.root.dot
