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
randomseed=$5
pathToMSworkspace=$6
outputPath=$7

randomseed=$((randomseed+1))
echo "changing directory to $outputPath"
cd $outputPath
pwd
if [[ -f $toyDatacard ]]; then
  combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength -s $((randomseed)) $toyDatacard"
  echo "$combineCmd"
  eval $combineCmd
  if [[ -f *.root.dot ]]; then
    rm *.root.dot
  fi

  toyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$((signalStrength)).GenerateOnly.mH125.$((randomseed)).root"
  echo "$toyFile"

  if [[ -f $toyFile ]]; then
    combineCmd="combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard"
    echo "$combineCmd"
    eval $combineCmd

    if [[ -f $pathToMSworkspace ]]; then
      combineCmd="combine -M MaxLikelihoodFit -m 125 -n _MS_mlfit --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $pathToMSworkspace"
      echo "$combineCmd"
      eval $combineCmd

      combineCmd='combine -M MultiDimFit '$pathToMSworkspace' --algo=grid --points=400 --minimizerStrategy 1 --minimizerTolerance 0.3 --cminApproxPreFitTolerance=25 --cminFallbackAlgo "Minuit2,migrad,0:0.3" --cminOldRobustMinimize=0 --X-rtd MINIMIZER_MaxCalls=9999999 -n _MS_multidim --saveWorkspace -m 125 -t '$numberOfToysPerExperiment' --toysFile '$toyFile' --saveFitResult'
      echo "$combineCmd"
      eval $combineCmd
    fi
    #

  else
    echo "Could not find toyFile, skipping the fit"
  fi

else
  echo "Could not find datacard for toy generation! Aborting..."
fi
