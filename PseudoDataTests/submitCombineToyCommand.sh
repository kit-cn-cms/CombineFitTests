#!/bin/bash

outputDir=$1
numberOfExperiments=$2
numberOfToysPerExperiment=1

datacardDir=/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards

targetDatacard="$datacardDir"/limits_Spring17v2p2_ttbarincl_datacard_ljets_jge6_tge4_hdecay_ttHbb_ttbb.txt

pathToCombineToyScript=/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests

mkdir -p "$outputDir"
cd "$outputDir"

for (( signalStrength = 0; signalStrength < 2; signalStrength++ )); do
  mkdir -p sig$signalStrength
  cd sig$signalStrength

  for (( i = 0; i <$numberOfExperiments; i++ )); do
    mkdir -p PseudoExperiment$i
    cd PseudoExperiment$i
    qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log.out -e log.err "$pathToCombineToyScript"/createCombineToy.sh $targetDatacard $numberOfToysPerExperiment $signalStrength $i
    #combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-5.00 --rMax=5.00 -t 1 --toysFrequentist --expectSignal 0 $targetDatacard
    cd ../
  done
  cd ../
done
