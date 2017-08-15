#!/bin/bash

targetDatacard=$1
toyDatacard=$2
numberOfToysPerExperiment=$3
signalStrength=$4
lowerBound=$5
upperBound=$6
pathToMSworkspace=$7
outputPath=$8
#pwd
workdir="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

if [[ -d $outputPath ]]; then
  cd $outputPath

  echo "starting PseudoExperiment generation"
  for (( i = $lowerBound; i < $upperBound; i++ )); do
    mkdir -p PseudoExperiment$i
    cd PseudoExperiment$i

    eval "$workdir/generateToysAndFits.sh $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $i $pathToMSworkspace $outputPath/PseudoExperiment$i"

    cd ../
  done
else
  echo "$outputPath is not a directory! Aborting"
fi
