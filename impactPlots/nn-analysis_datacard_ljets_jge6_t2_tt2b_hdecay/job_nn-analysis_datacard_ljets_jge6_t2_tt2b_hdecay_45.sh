#!/bin/sh
ulimit -s unlimited
cd /nfs/dust/cms/user/mharrend/doktorarbeit/combine-tool/CMSSW_7_4_7/src
export SCRAM_ARCH=slc6_amd64_gcc491
eval `scramv1 runtime -sh`
cd /nfs/dust/cms/user/mharrend/doktorarbeit/CombineFitTests/PseudoDataTests/scripts/impactPlots/nn-analysis_datacard_ljets_jge6_t2_tt2b_hdecay
eval combine -M MultiDimFit -n _paramFit_nn-analysis_datacard_ljets_jge6_t2_tt2b_hdecay_CMS_scale_PileUpPtBB_j --algo impact --redefineSignalPOIs r -P CMS_scale_PileUpPtBB_j --floatOtherPOIs 1 --saveInactivePOI 1 --robustFit 1 --rMin -10 --rMax 10 -m 125 -d /nfs/dust/cms/user/mharrend/doktorarbeit/output201710251152-reference-nn-221646/datacards/nn-analysis_datacard_ljets_jge6_t2_tt2b_hdecay.root
