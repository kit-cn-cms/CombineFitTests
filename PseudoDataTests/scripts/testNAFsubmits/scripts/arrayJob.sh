#!/bin/bash 
subtasklist=(
/nfs/dust/cms/user/vdlinden/CombineFitTests/PseudoDataTests/scripts/testNAFsubmits/scripts/test1_array.sh 
/nfs/dust/cms/user/vdlinden/CombineFitTests/PseudoDataTests/scripts/testNAFsubmits/scripts/test2_array.sh 
)
thescript=${subtasklist[$SGE_TASK_ID]}
echo "${thescript}"
. $thescript