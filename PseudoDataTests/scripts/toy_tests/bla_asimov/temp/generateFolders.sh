#!/bin/bash

signalStrength=$1
lowerBound=$2
upperBound=$3
outputPath=$4
numberOfToysPerExperiment=1

if [[ -d $outputPath ]]; then
	cd $outputPath

	echo "starting PseudoExperiment generation"
	for (( i = $((lowerBound+1)); i <= $upperBound; i++ )); do
		mkdir -p PseudoExperiment$i
		if [[ -d PseudoExperiment$i ]]; then
			cd PseudoExperiment$i

			eval "/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateToysAndFits.sh $signalStrength $((i+0)) $outputPath/PseudoExperiment$i $numberOfToysPerExperiment"

			cd ../

		else
			 echo "Could not generate folder PseudoExperiment$i in $outputPath"
		fi

	done
else
	echo "$outputPath is not a directory! Aborting"
fi