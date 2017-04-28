#!/bin/bash

outputDir=$1
numberOfExperiments=$2
experimentsPerJob=$3
numberOfToysPerExperiment=1

datacardDir=/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards

targetDatacard="$datacardDir"/limits_Spring17v2p2_ttbarincl_datacard_ljets_jge6_tge4_hdecay_ttHbb_ttbb.root

pathToCombineToyScript=/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests

mkdir -p "$outputDir"
cd "$outputDir"

numberOfLoops=$((numberOfExperiments/experimentsPerJob))
rest=$((numberOfExperiments%experimentsPerJob))

for (( signalStrength = 0; signalStrength < 2; signalStrength++ )); do
  mkdir -p sig$signalStrength
  cd sig$signalStrength

  for (( i = 0; i <$numberOfLoops; i++)); do
    upperBound=$(((i+1)*experimentsPerJob))
    lowerBound=$((i*experimentsPerJob))
    qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log.out -e log.err "$pathToCombineToyScript"/createCombineToy.sh $targetDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound
  done
  upperBound=$numberOfExperiments
  lowerBound=$((numberOfExperiments-rest))
  qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log.out -e log.err "$pathToCombineToyScript"/createCombineToy.sh $targetDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound
  cd ../
done
