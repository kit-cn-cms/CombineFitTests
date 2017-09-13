#!/bin/bash

targetDatacard=$1
toyDatacard=$2
outputDir=$3
numberOfExperiments=$4
experimentsPerJob=$5
numberOfToysPerExperiment=$6
<<<<<<< HEAD
listOfPOIs=$7

=======
pathToMSworkspace=$7
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
#datacardDir=/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards


pathToCombineToyScript=/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts

mkdir -p "$outputDir"
cd "$outputDir"

numberOfLoops=$((numberOfExperiments/experimentsPerJob))
rest=$((numberOfExperiments%experimentsPerJob))

for (( signalStrength = 0; signalStrength < 2; signalStrength++ )); do
  mkdir -p sig$signalStrength
  cd sig$signalStrength

  echo "creating asimov data set"
  mkdir -p asimov
  cd asimov

<<<<<<< HEAD
  combineCmd="qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_asimov.out -e log_asimov.err $pathToCombineToyScript/generateToysAndFits.sh $targetDatacard $toyDatacard -1 $signalStrength 123456 $listOfPOIs"
  echo $combineCmd
=======
  combineCmd="qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_asimov.out -e log_asimov.err $pathToCombineToyScript/generateToysAndFits.sh $targetDatacard $toyDatacard -1 $signalStrength 123456 $pathToMSworkspace ./"
  #echo $combineCmd
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
  eval $combineCmd
  cd ../

  for (( i = 0; i <$numberOfLoops; i++)); do
    upperBound=$(((i+1)*experimentsPerJob))
    lowerBound=$((i*experimentsPerJob))
<<<<<<< HEAD
    qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_$((lowerBound))To$((upperBound)).out -e log_$((lowerBound))To$((upperBound)).err "$pathToCombineToyScript"/'createFoldersAndDoToyFits.sh' $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound $listOfPOIs
  done
  upperBound=$numberOfExperiments
  lowerBound=$((numberOfExperiments-rest))
  qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_$((lowerBound))To$((upperBound)).out -e log_$((lowerBound))To$((upperBound)).err "$pathToCombineToyScript"/'createFoldersAndDoToyFits.sh' $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound $listOfPOIs
=======
    qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_$((lowerBound))To$((upperBound)).out -e log_$((lowerBound))To$((upperBound)).err "$pathToCombineToyScript"/'createFoldersAndDoToyFits.sh' $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound $pathToMSworkspace ./
  done
  upperBound=$numberOfExperiments
  lowerBound=$((numberOfExperiments-rest))
  qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log_$((lowerBound))To$((upperBound)).out -e log_$((lowerBound))To$((upperBound)).err "$pathToCombineToyScript"/'createFoldersAndDoToyFits.sh' $targetDatacard $toyDatacard $numberOfToysPerExperiment $signalStrength $lowerBound $upperBound $pathToMSworkspace ./
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
  cd ../
done
