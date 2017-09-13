#!/bin/bash
<<<<<<< HEAD

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
listOfPOIs=$6

pathToConvertMDFtoMLF="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/convertMDFtoMLF.py"


if [[ -f $toyDatacard ]]; then
  combineCmd="combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength -s $randomseed $toyDatacard"
  echo "$combineCmd"
  eval $combineCmd
  if [[ -f *.root.dot ]]; then
    rm *.root.dot
  fi

  toyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$((signalStrength)).GenerateOnly.mH125.$((randomseed)).root"
  echo "$toyFile"

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
=======
pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW.txt"
pathToNLLscanner="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/plotting/nllscan.py"
if [[ -f "$pathToCMSSWsetup" ]]; then

  eval "source $pathToCMSSWsetup"
  targetDatacard=$1
  toyDatacard=$2
  numberOfToysPerExperiment=$3
  signalStrength=$4
  randomseed=$5
  pathToMSworkspace=$6
  outputPath=$7

  randomseed=$((randomseed+1))
  echo "input variables:"
  echo "targetDatacard = $1"
  echo "toyDatacard = $2"
  echo "#Toys/Experiment = $3"
  echo "mu = $4"
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
        combineCmd="combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard"
        echo "$combineCmd"
        eval $combineCmd

        if [[ -f "higgsCombineTest.MaxLikelihoodFit.mH125.123456.root" ]]; then
          rm "higgsCombineTest.MaxLikelihoodFit.mH125.123456.root"
        fi

        if ! [[ -f "mlfit.root" ]]; then
          echo "could not produce mlfit.root file!"
        fi

        if [[ -f $pathToMSworkspace ]]; then
          echo "starting multiSignal analysis"
          combineCmd="combine -M MaxLikelihoodFit -m 125 -n _MS_mlfit --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $pathToMSworkspace"
          echo "$combineCmd"
          eval $combineCmd

          if [[ -f "higgsCombine_MS_mlfit.MaxLikelihoodFit.mH125.123456.root" ]]; then
            rm "higgsCombine_MS_mlfit.MaxLikelihoodFit.mH125.123456.root"
          fi

          if ! [[ -f "mlfit_MS_mlfit.root" ]]; then
            echo "could not produce mlfit_MS_mlfit.root file!"
          fi

        fi


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
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
fi
