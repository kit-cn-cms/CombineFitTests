#!/bin/bash
pathToCMSSWsetup=/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW_8_1_0.txt
if [[ -f "$pathToCMSSWsetup" ]]; then

	eval "source $pathToCMSSWsetup"
	targetDatacard=/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/signal_background_example/datacard.root
	toyDatacard=/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/signal_background_example/datacard.root
	signalStrength=$1
	randomseed=$2
	outputPath=$3
	numberOfToysPerExperiment=$4

#___________________________________________________
	echo "input variables:"
	echo "targetDatacard = $targetDatacard"
	echo "toyDatacard = $toyDatacard"
	echo "#Toys/Experiment = $numberOfToysPerExperiment"
	echo "mu = $signalStrength"
	echo "randomseed = $randomseed"
	echo "pathToMSworkspace = $pathToMSworkspace"
	echo "outputPath = $outputPath"

	echo "changing directory to $outputPath"
	if [[ -d "$outputPath" ]]; then
		cd $outputPath

		if [[ -f $toyDatacard ]]; then
			combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength -s $((randomseed)) $toyDatacard"
			echo "$combineCmd"
			eval $combineCmd

			if [[ -f *.root.dot ]]; then
				rm *.root.dot
			fi

			toyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$signalStrength.GenerateOnly.mH125.$((randomseed)).root"
			echo "$toyFile"
			if [[ -f $toyFile ]]; then
				combineCmd="combine -M MaxLikelihoodFit -m 125 --cminFallbackAlgo Minuit2,migrad,0:0.00001 --cminDefaultMinimizerStrategy 0 --cminDefaultMinimizerTolerance 1e-5 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard"
				echo "$combineCmd"
				eval $combineCmd

				if ! [[ -f "mlfit.root" ]]; then
					echo "could not produce mlfit.root file!"
				fi
				for f in higgsCombine*MaxLikelihoodFit*.root; do
					if [[ -f "$f" ]]; then
						rm "$f"
					fi

				done
			else
				echo "Could not find toyFile, skipping the fit"
			fi

		else
			echo "Could not find datacard for toy generation! Aborting..."
		fi

	else
		echo "$outputPath is not a directory! Aborting"
	fi

else
	echo "Could not find file to setup CMSSW! Aborting"
fi