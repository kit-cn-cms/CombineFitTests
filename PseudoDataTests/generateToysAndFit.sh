#!/bin/bash

CMSSW_BASE=/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
cd $CMSSW_BASE
eval `scram runtime -sh`
cd -

targetDatacard=$1
toyDatacard=$2
numberOfToysPerExperiment=$3
signalStrength=$4
lowerBound=$5
upperBound=$6

#pwd
for (( i = $lowerBound; i <= $upperBound; i++ )); do
  mkdir -p PseudoExperiment$i
  cd PseudoExperiment$i
  if [[ -f $toyDatacard ]]; then
    combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength -s $i $toyDatacard"
    echo "$combineCmd"
    eval $combineCmd
    if [[ -f *.root.dot ]]; then
      rm *.root.dot
    fi

    toyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$((signalStrength)).GenerateOnly.mH125.$((i)).root"

    if [[ -f $toyFile ]]; then
      combineCmd="combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard"
      echo "$combineCmd"
      eval $combineCmd
    else
      echo "Could not find toyFile, skipping the fit"
    fi

  else
    echo "Could not find datacard for toy generation! Aborting..."
  fi

  cd ../
done
