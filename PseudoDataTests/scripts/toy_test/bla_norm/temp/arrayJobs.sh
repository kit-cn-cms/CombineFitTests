#!/bin/bash 
subtasklist=(
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/temp/generateToysAndFits.sh 2.0 123456 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/sig2.0/asimov -1' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/temp/generateFolders.sh 2.0 0 30 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/sig2.0' 
'/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/temp/generateFolders.sh 2.0 30 50 /nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/sig2.0' 
)
thescript=${subtasklist[$SGE_TASK_ID-1]}
thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`
echo "${thescript}
echo "${thescriptbasename}
. $thescript 1>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/temp/logs/$JOB_ID.$SGE_TASK_ID.o 2>>/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toy_test/bla_norm/temp/logs/$JOB_ID.$SGE_TASK_ID.e
