#!/bin/bash

targetDatacard=$1
toyDatacard=$2
numberOfToysPerExperiment=$3
signalStrength=$4
lowerBound=$5
upperBound=$6
pathToMSworkspace=$7
#pwd
workdir="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

for (( i = $lowerBound; i < $upperBound; i++ )); do
  mkdir -p PseudoExperiment$i
  cd PseudoExperiment$i

  eval $workdir/generateToysAndFits.sh $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $i $pathToMSworkspace

  cd ../
done
