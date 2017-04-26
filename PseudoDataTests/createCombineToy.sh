#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

targetDatacard=$1
numberOfToysPerExperiment=$2
signalStrength=$3
randomSeed=$4

#pwd
combineCmd="combine -v 99 -M MaxLikelihoodFit -m 125 --freezeNuisances all --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-5.00 --rMax=5.00 -t $2 --minos all -s $randomSeed --expectSignal $signalStrength $targetDatacard"
echo "$combineCmd"
eval $combineCmd
rm *.root.dot
