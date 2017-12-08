#!/bin/bash 
subtasklist=(
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateToysAndFits.sh 0.0 123456 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig0.0/asimov -1' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 0.0 0 30 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig0.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 0.0 30 60 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig0.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 0.0 60 90 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig0.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 0.0 90 100 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig0.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateToysAndFits.sh 1.0 123456 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig1.0/asimov -1' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 1.0 0 30 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 1.0 30 60 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 1.0 60 90 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig1.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/generateFolders.sh 1.0 90 100 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/sig1.0' 
)
thescript=${subtasklist[$SGE_TASK_ID-1]}
thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`
echo "${thescript}
echo "${thescriptbasename}
. $thescript 1>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/logs/$JOB_ID.$SGE_TASK_ID.o 2>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_tests/bla_asimov/temp/logs/$JOB_ID.$SGE_TASK_ID.e
