import os
import sys
import glob
from subprocess import call

wildcard = sys.argv[1]
if len(sys.argv) > 2 and sys.argv[2] == "direct":
    pathToScript = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/runPlotResults.sh"
else:
    pathToScript = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/submitScript.sh"

for folder in glob.glob(wildcard):
    folder = os.path.abspath(folder)
    if len(glob.glob(folder+"/sig*/*/mlfit.root")) > 1800:# and len(glob.glob(folder+"/sig*/*/mlfit_MS_mlfit.root")) > 1800:
        print "start plotResults for", folder
        pathToShapeExp = ""
        if os.path.exists(folder+"/temp/temp_shape_expectation.root"):
            pathToShapeExp = folder+"/temp/temp_shape_expectation.root"
        call([pathToScript + " " + folder + " " + pathToShapeExp],shell=True)
    else:
        print "skipping", folder
        print "\t # mlfit.root:", len(glob.glob(folder+"/sig*/*/mlfit.root")), "\t # mlfit_MS_mlfit.root", len(glob.glob(folder+"/sig*/*/mlfit_MS_mlfit.root"))
