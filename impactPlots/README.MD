# Impact Plots script

Repository based on work by Philip

These tools use the CombineHarvester. For more information see [here](https://twiki.cern.ch/twiki/bin/view/CMS/HiggsWG/SWGuideNonStandardCombineUses#Nuisance_parameter_impacts)

Please set the CMSSW environment before using the tools.

### Stages for Impact Plot Generation

#### Step 1: submitImpactFits.py

  * First parameter is a string wildcard to the datacards/workspaces that are to be analyzed (uses python module `glob`)
  * All following parameters are additional combine options
  * Example for fitting to asimov data with #mu = 1: `python submitImpactFits.py "wildcard/to/data" -t -1 --expectSignal 1 --minimizerStrategy 0 --minimizerTolerance 0.00001`
  * The script will generate a folder based on the datacard name in the current working directory.
   If the input file is not a .root file (-> workspace) it will call `text2workspace.py` to generate a workspace in the datacard directory.
   The computing time can be significantly reduced if input is workspace.
   First, the script will perform an initial fit of the datacard. Afterwards, a fit for each parameter will be performed via the batch system, respectively.
   You can configure the submit option in the [batchConfig class](https://gitlab.cern.ch/kit-cn-cms/impactPlots/blob/b76e641f30d4731ac1969ffb5a320339ecc9dc55/base/batchConfig.py).
   There will be a `commands.txt` file in each folder containing the commands used to generate the present files.
   A .json file containing the list of datacards, the list of generated folders and the list of additional commands is generated in the current working directory. 
   
#### Step 2: autoResubmitImpactFits.py
  * If only one parameter is given, the script expects the output .json file generated in step 1
  * Else:
    * First parameter is string wildcard to folders with impact fits that are to be checked
    * Second parameter is string wildcard to datacards or workspaces for which there should be impact plots
    * All following parameters are additional combine options. These should be the same as in step 1 in order to generate consistent impacts
  * Example: `python autoResubmitImpactFits.py "wildcard/to/impact/folders" "wildcard/to/data" -t -1 --expectSignal 1 --minimizerStrategy 0 --minimizerTolerance 0.00001`
  * The script will first check for .sh files and then compare the number of shell scripts with the number of rootfile in the corresponding directory.
   If these numbers are not equal, it will call the corresponding CombineHarvester command to regenerate the shell scripts.
   If the numbers are equal the script will loop over all .root files to check whether they are intact or broken.
   If it encounters a broken file it will automatically search for the corresponding shell script. The scripts are collected and finally submitted as an array job to the batch system (if possible).
   Finally, the script cross checks the list of folders with impact fits with the given list of datacards/workspace to see if anything is missing.
   
   All is well if nothing has to be resubmitted.  
   
#### Step 3: drawImpactPlots.py
  
  * First parameter is string wildcard to folders with impact fits that are to be checked
  * Second parameter is the path containing the workspaces corresponding to the impact fit folders
  * Example: `python drawImpactPlots.py "wildcard/to/impact/folders" path/containing/datacards`
  * The script will try to find the workspace based on the impact folder name á la `datacard_dirname/impact_folder_name.root`
   If the workspace was found the script will create a .json file containing the impact values and the impact plots.
   Each page of the output .pdf file shows 30 parameters.
   
### Useful Scripts

#### findOnesidedNP.py

  * Takes wildcard to .json files to analyze. Can also handle not string wildcards
  * Example: `python findOnesidedNP.py path/to/impact/folders/*.json`
  * The script will go through all .json files and look for parameters with one-sided impacts on the signal strength. Output in each folder: onesidedNPs.txt

#### countOnesidedParams.py

  * Takes wildcard to .txt files created with findOnesidedNP.py
  * Example: `python countOnesidedParams.py wildcard/to/txt/files/onesidedNPs.txt`
  * The script loops over all given .txt files and counts the frequency of each parameter respectively
  
#### createLatexSlides.py

  * Takes wildcard to impact plot .pdfs
  * Example: `python createLatexSlides.py path/to/impact/plots/*.pdf`
  * The script creates a .tex file containing frames for each page of the impact plot. The generated slides use latex package `pdfpages`
