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
lowerBound=$4
upperBound=$5

#pwd
for (( i = $lowerBound; i <= $upperBound; i++ )); do
  mkdir -p PseudoExperiment$i
  cd PseudoExperiment$i
  combineCmd="combine -v 99 -M MaxLikelihoodFit --toysNoSystematics --freezeNuisances all -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-5.00 --rMax=5.00 -t $2 --minos all -s $i --expectSignal $signalStrength $targetDatacard"
  echo "$combineCmd"
  eval $combineCmd
  rm *.root.dot
  cd ../
done
