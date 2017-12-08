#!/bin/bash 
subtasklist=(
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/generateToysAndFits.sh 1.0 123456 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/sig1.0/asimov -1' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/generateFolders.sh 1.0 0 30 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/generateFolders.sh 1.0 30 60 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/generateFolders.sh 1.0 60 90 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/generateFolders.sh 1.0 90 100 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/sig1.0' 
)
thescript=${subtasklist[$SGE_TASK_ID-1]}
thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`
echo "${thescript}
echo "${thescriptbasename}
. $thescript 1>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/logs/$JOB_ID.$SGE_TASK_ID.o 2>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov_2/temp/logs/$JOB_ID.$SGE_TASK_ID.e
