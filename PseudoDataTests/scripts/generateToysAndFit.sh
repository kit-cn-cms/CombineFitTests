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
listOfPOIs=$7

pathToConvertMDFtoMLF="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/convertMDFtoMLF.py"
#pwd


for (( i = $lowerBound; i < $upperBound; i++ )); do
  mkdir -p PseudoExperiment$i
  cd PseudoExperiment$i
  if [[ -f $toyDatacard ]]; then
    combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength --toysFrequentist -s $i $toyDatacard"
    echo "$combineCmd"
    eval $combineCmd
    if [[ -f *.root.dot ]]; then
      rm *.root.dot
    fi

    toyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$((signalStrength)).GenerateOnly.mH125.$((i)).root"

    if [[ -f $toyFile ]]; then
      combineCmd="combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 --floatAllNuisances 1 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard"
      echo "$combineCmd"
      eval $combineCmd
      # combineCmd='combine -M MultiDimFit '$targetDatacard' --algo=none --minimizerStrategy 1 --minimizerTolerance 0.3 --cminApproxPreFitTolerance=25 --cminFallbackAlgo "Minuit2,migrad,0:0.3" --cminOldRobustMinimize=0 --X-rtd MINIMIZER_MaxCalls=9999999 -n _full_fit --saveWorkspace -m 125 --redefineSignalPOIs '$listOfPOIs' -t 1 --toysFile '$toyFile' --saveFitResult'
      # echo "$combineCmd"
      # eval $combineCmd
      # python $pathToConvertMDFtoMLF
    else
      echo "Could not find toyFile, skipping the fit"
    fi

  else
    echo "Could not find datacard for toy generation! Aborting..."
  fi

  cd ../
done
