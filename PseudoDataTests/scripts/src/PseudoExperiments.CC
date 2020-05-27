#include "../interface/PseudoExperiments.h"
#include "ShapeContainer.CC"

void printTime(TStopwatch& watch, TString text){
  std::cout << text.Data() << "\n\tReal Time: " << watch.RealTime() << "\tCPU Time: " << watch.CpuTime() << std::endl;

}

PseudoExperiments::PseudoExperiments(const TString& label, const Double_t injectedMu, const bool noBinByBin)
: debug_(false),
fitBmustConverge_(false),
fitSBmustConverge_(true),
accurateCovariance_(true),
noBinByBin_(noBinByBin),
label_(label), injectedMu_(injectedMu), color_(kGray){
  if( debug_ ) std::cout << "DEBUG " << this << ": constructor" << std::endl;
}

PseudoExperiments::~PseudoExperiments() {
  if( debug_ ) std::cout << "DEBUG " << this << ": destructor" << std::endl;

}

void PseudoExperiments::convertToHisto(std::map<TString, std::map<TString,std::vector<double> > >& vecs, std::map<TString, std::map<TString,TH1*> >& histos, const TString title = "") const{
  TString tempHistoName;
  TString parnameOne;
  TString parnameTwo;
  for(std::map<TString, std::map<TString,std::vector<double> > >::const_iterator it = vecs.begin(); it != vecs.end(); it++){
    for(std::map<TString,std::vector<double> >::const_iterator subit = it->second.begin(); subit != it->second.end(); subit++)
    {
      parnameOne = it->first.Data();
      parnameTwo = subit->first.Data();
      TUUID id;
      tempHistoName.Form("%s:%s_%s_correlations_%s", label_.Data(), parnameOne.Data(), parnameTwo.Data(), id.AsString());
      if(tempHistoName.Contains(" = ")) tempHistoName.ReplaceAll(" = ", "_");
      if(tempHistoName.Contains(" ")) tempHistoName.ReplaceAll(" ", "_");
      histos[parnameOne][parnameTwo] = helperFuncs::createHistoFromVector(tempHistoName, subit->second, title, -1, 1);
    }
  }
  vecs.clear();
}

void PseudoExperiments::readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it) const{
  RooRealVar* param = NULL;

  // if(init) TIter it = result->floatParsInit().createIterator();
  // else TIter it = result->floatParsFinal().createIterator();
  while((param = (RooRealVar*)it.Next())){
    if(!helperFuncs::compare_classname(param, "RooRealVar")) continue;
    //Double_t value = param->getVal();
    
    helperFuncs::find_and_fill_histo(hists, param->GetName(), param->getVal(), debug_);
    
    helperFuncs::find_and_fill_histo(hErrorsHi, param->GetName(), param->getErrorHi(), debug_);
  
    helperFuncs::find_and_fill_histo(hErrorsLo, param->GetName(), param->getErrorLo(), debug_);
  
    helperFuncs::find_and_fill_histo(hErrors, param->GetName(), param->getError(), debug_);

  }
}

void PseudoExperiments::readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it, std::map<TString, TH1*>& biasMap) const{
  RooRealVar* param = NULL;

  // if(init) TIter it = result->floatParsInit().createIterator();
  // else TIter it = result->floatParsFinal().createIterator();
  double bias = 0;
  std::cout << "entering bias code\n";
  while((param = (RooRealVar*)it.Next())){
    if(!helperFuncs::compare_classname(param, "RooRealVar")) continue;
    //Double_t value = param->getVal();
    
    helperFuncs::find_and_fill_histo(hists, param->GetName(), param->getVal(), debug_);
    
    helperFuncs::find_and_fill_histo(hErrorsHi, param->GetName(), param->getErrorHi(), debug_);
  
    helperFuncs::find_and_fill_histo(hErrorsLo, param->GetName(), param->getErrorLo(), debug_);
  
    helperFuncs::find_and_fill_histo(hErrors, param->GetName(), param->getError(), debug_);

    if(std::find(pois_.begin(), pois_.end(), param->GetName()) != pois_.end()){
      if(param->getError() == 0) {
        std::cerr << "ERROR: Could not calculate bias, error is 0 for parameter " << param->GetName() << std::endl;
        continue;
      }
      bias = (param->getVal()-injectedMu_)/param->getError();
      std::cout << "\tfilling bias for param " << param->GetName() << " ("<< bias << ")\n";
      helperFuncs::find_and_fill_histo(biasMap, param->GetName(), bias, debug_);
    }
  }
}

void PseudoExperiments::readRooRealVar(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TIter it) const{
  RooRealVar* param = NULL;

  while((param = (RooRealVar*)it.Next())){
    if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
    if(debug_) std::cout << "looking for parameter " << param->GetName() <<std::endl;
    histVecs[param->GetName()].push_back(param->getVal());
    hErrorVecs[param->GetName()].push_back(param->getError());
    hErrorsHiVecs[param->GetName()].push_back(param->getErrorHi());
    hErrorsLoVecs[param->GetName()].push_back(param->getErrorLo());
  }
}

void PseudoExperiments::readRooRealVar(TH1* hist, const RooFitResult* result, const TString& currentVarName) const{

  RooRealVar* param = NULL;

  param = (RooRealVar*)result->floatParsFinal().find( currentVarName.Data() );
  if( (helperFuncs::compare_classname(param, "RooRealVar")) )
  {
    if(param != NULL){
      Double_t value = param->getVal();
      hist->Fill(value);
    }
  }
}

void PseudoExperiments::readRooRealVar(std::vector<Double_t>& histVec, const RooFitResult* result, const TString& currentVarName) const{

  RooRealVar* param = NULL;

  param = (RooRealVar*)result->floatParsFinal().find( currentVarName.Data() );
  if((helperFuncs::compare_classname(param, "RooRealVar")))
  {
    if(param != NULL){
      Double_t value = param->getVal();
      histVec.push_back(value);
    }
  }
}

void PseudoExperiments::addExperimentFromRoofit(const TString& mlfit) {
  if( debug_ ) std::cout << "DEBUG: addExperiment: " << mlfit << std::endl;
  TFile* file = new TFile(mlfit,"READ");
  if (file != NULL)
  {
    if( !file->IsOpen() || file->IsZombie()) {
      std::cerr << "ERROR opening file '" << mlfit << "'" << std::endl;
      //throw std::exception();
    }
    else {
      //check if the s+b and the b fit converged
      if(!file->TestBit(TFile::kRecovered))
      {
        bool storeExperiment = true;
        TStopwatch overallTime;
        overallTime.Start();
        if(fitSBmustConverge_ || fitBmustConverge_) storeExperiment = checkFitStatus(file);
        if(storeExperiment && accurateCovariance_) storeExperiment = checkCovarianceMatrix(file);
        // store POI value
        if(storeExperiment)
        {
          TStopwatch watch;

          // store nuisance parameter values
          if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
          if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
          const RooFitResult* result = NULL;
          result = (RooFitResult*)file->Get("fit_b");

          if( result == 0 ) {
            std::cerr << "ERROR getting 'fit_b' from file '" << file->GetName() << "'" << std::endl;
            //throw std::exception();
          }
          else{
            // storeRooFitResults(valuesPostfitB_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_, file, result, correlationsPostfitB_);
            storeRooFitResults(valuesPostfitB_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_, file, result, correlationvalsPostfitB_);
          }
          if(result != NULL){
            if(debug_) std::cout << "deleting RooFitResult Pointer fit_b\n";

            delete result;
            result = NULL;
          }
          result = (RooFitResult*)file->Get("fit_s");

          if( result == 0 ) {
            std::cerr << "ERROR getting 'fit_s' from file '" << file->GetName() << "'" << std::endl;
            //throw std::exception();
          }
          else{
            if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
            // storeRooFitResults(valuesPostfitS_, valuesPostfitSerrors_, valuesPostfitSerrorHi_, valuesPostfitSerrorLo_, file, result, correlationsPostfitS_);
            storeRooFitResults(valuesPostfitS_, valuesPostfitSerrors_, valuesPostfitSerrorHi_, valuesPostfitSerrorLo_, file, result, correlationvalsPostfitS_);
            if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;

            
            if( debug_ ) std::cout << "    DEBUG: prefit values" << std::endl;
            storePrefitValues(valuesPrefit_, valuesPrefiterrors_, valuesPrefiterrorHi_, valuesPrefiterrorLo_, file);
            if(result != NULL) {
              if(debug_) std::cout << "deleting RooFitResult Pointer fit_s";
              delete result;
            }
          }

          

          if(debug_) std::cout << "  DEBUG: store shapes\n";
          if(debug_) std::cout << "\tDEBUG: prefit shapes\n";
          storeShapes(prefitShapes_, file, "shapes_prefit");
          if(debug_) std::cout << "\tDEBUG: postfit B shapes\n";
          storeShapes(postfitBshapes_, file, "shapes_fit_b");
          if(debug_) std::cout << "\tDEBUG: postfit S shapes\n";
          storeShapes(postfitSshapes_, file, "shapes_fit_s");
          if( debug_ ) std::cout << "  DEBUG: done storing shapes" << std::endl;

          //delete result_tree;
        }
        //}
        else std::cout << "skipping experiment in '" << mlfit.Data() << "'\n";
        overallTime.Stop();
        if(debug_) printTime(overallTime, "Time it took to process " + mlfit);
      }
      else std::cerr << "  ERROR while opening file " << mlfit.Data() << ", skipping...\n";
      file->Close();

    }
    if(file != NULL) {
      delete file;
      file = NULL;
    }
  }
}

void PseudoExperiments::readCorrTree(TTree* tree, std::map<TString, std::map<TString,TH1*> >& correlations){
  if(tree){
    TString histName;
    TString branchName;
    TString treeName = tree->GetName();
    TObjArray* array = tree->GetListOfBranches();
    // std::map<TString, std::map<TString, std::map<TString,TH1*> > >::const_iterator it = correlations.find(treeName.Data());
    for(int i=0; i<array->GetEntries(); i++){
      const TUUID id;
      branchName = array->At(i)->GetName();
      histName.Form("%s_%s_correlations_%s", treeName.Data(), branchName.Data(), id.AsString());
      std::cout << "Correlations for parameter " << treeName << ":\n";
      helperFuncs::saveTreeVals(tree, branchName, correlations[treeName], histName, nBins_, -1.2, 1.2);
    }
  }
}

bool PseudoExperiments::readCorrFolder(TFile* infile, const TString& folderName, std::map<TString, std::map<TString,TH1*> >& correlations){
  if(infile->cd(folderName.Data())){
    std::cout << "loading correlations from folder " << folderName << std::endl;
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        readCorrTree(tree, correlations);
      }
    }
    infile->cd();
    return true;
  }
  else{
    std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
    return false;
  }
}

void PseudoExperiments::readParamTree(TTree* tree, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo){
  if(tree)
  {
    TString paramName = tree->GetName();
    TString histoTitle;
    histoTitle.Form(";%s;Frequency", paramName.Data());
    hists[paramName] = helperFuncs::branch2histo(tree, "Value", histoTitle);
    histoTitle.Form(";%s error;Frequency", paramName.Data());
    hErrors[paramName] = helperFuncs::branch2histo(tree, "Error", histoTitle);
    histoTitle.Form(";%s high error;Frequency", paramName.Data());
    hErrorsHi[paramName] = helperFuncs::branch2histo(tree, "High Error", histoTitle);
    histoTitle.Form(";%s low error;Frequency", paramName.Data());
    hErrorsLo[paramName] = helperFuncs::branch2histo(tree, "Low Error", histoTitle);
  }
}

bool PseudoExperiments::readParamFolder(TFile* infile, const TString& folderName, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo){
  if(infile->cd(folderName.Data())){
    std::cout << "saving parameter values in folder " << folderName.Data() << std::endl;
    bool saveNps = nps_.empty();
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        
        if(saveNps) 
        {
            std::cout << "saving nuisance parameter " << treeName << std::endl;
            if(!((treeName.Contains("prop_bin") || treeName.Contains("_Bin")) && noBinByBin_) && !(treeName.BeginsWith("r"))) nps_.push_back(treeName);
        }
        else{
            if( std::find( nps_.begin(), nps_.end(), treeName) == nps_.end() && !((treeName.Contains("prop_bin") || treeName.Contains("_Bin")) && noBinByBin_) ){
                pois_.push_back(treeName);
            }
        }
        std::cout << "reading nuisance parameter " << treeName << std::endl;
        readParamTree(tree, hists, hErrors, hErrorsHi, hErrorsLo);
      }
    }
    return true;
  }
  else{
    std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
    return false;
  }
}

void PseudoExperiments::readShapeTree(TTree* tree, std::vector<ShapeContainer*>& shapeVec){
  if(tree){
    TString treeName = tree->GetName();
    std::cout << "creating container for category " << treeName << std::endl;

    ShapeContainer* container = new ShapeContainer(treeName);
    container->loadShapes(tree, injectedMu_);
    shapeVec.push_back(container);
    
  }
}

bool PseudoExperiments::readShapeFolder(TFile* infile, const TString& folderName, std::vector<ShapeContainer*>& shapeVec){
  if(infile->cd(folderName.Data())){
    std::cout << "loading shapes from folder " << folderName << std::endl;
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        readShapeTree(tree, shapeVec);
      }
    }
    return true;
  }
  else{
      std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
      return false;
  }
}

bool PseudoExperiments::addExperimentFromTree(const TString& mlfit){
  if(mlfit.EndsWith(".root")){
    TFile* infile = TFile::Open(mlfit);
    if(infile->IsOpen() && !infile->IsZombie() && !infile->TestBit(TFile::kRecovered))
    {
      if(!(readCorrFolder(infile, "Correlation_sig", correlationsPostfitS_)) ) return false;
      if(!(readCorrFolder(infile, "Correlation_bac", correlationsPostfitB_)) ) return false;
      
      if(!(readParamFolder(infile, "signal", valuesPostfitS_, valuesPostfitSerrors_, valuesPostfitSerrorHi_, valuesPostfitSerrorLo_))) return false;
      if(!(readParamFolder(infile, "background", valuesPostfitB_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_))) return false;
      if(!(readParamFolder(infile, "Prefit", valuesPrefit_, valuesPrefiterrors_, valuesPrefiterrorHi_, valuesPrefiterrorLo_))) return false;
      if(!(readShapeFolder(infile, "shapes_fit_s", postfitSshapes_)) ) return false;
      if(!(readShapeFolder(infile, "shapes_fit_b", postfitBshapes_)) ) return false;
      if(!(readShapeFolder(infile, "shapes_prefit", prefitShapes_))) return false;
      return true;
    }
  }
  else{
    std::cerr << "ERROR:\tinput file is not a root file!";
  }
  return false;
}

void PseudoExperiments::addExperiments(TString& sourceDir, const TString& mlfitFile){
  /*
  Input:
  sourceDir: directory which contains PseudoExperiment folders
  mlfitFile: .root file which contains the fit results from the combine fit
  */
  
  if(sourceDir.EndsWith(".root")){
    TList* filelist;
    filelist = helperFuncs::interpretWildcards(sourceDir);
    TIter it(filelist);
    TObjString* obj;
    TString file;
    while((obj = (TObjString*)it()))
    {
      file = obj->GetName();
      std::cout << "loading all experiment data from " << file.Data() << std::endl;
      if(!addExperimentFromTree(file))
      {
        std::cout << "failed to find TTree with values, will try to load directly from RooFitResult next\n";
        addExperimentFromRoofit(file);
      }
    }
    for(auto* p : *filelist) delete p;
    delete filelist;
  }
  else if(sourceDir.EndsWith(".txt")){
    std::cout << "reading files from " << sourceDir << std::endl;
    ifstream input;
    input.open(sourceDir.Data());
    if( input.is_open()){
      std::string infile;
      while(std::getline(input, infile)){
        // input >> infile;
        std::cout << "loading all experiment data from " << infile << std::endl;
        // if(!addExperimentFromTree(infile))
        // {
        //   std::cout << "failed to find TTree with values, will try to load directly from RooFitResult next\n";
          addExperimentFromRoofit(infile);
        // }
      }
    }
    // exit(0);
  }
  else
  {
    //load PseudoExperiment folders
    if(sourceDir.EndsWith("/")) sourceDir.Chop();
  
    TSystemFile *pseudoExperimentFolder;
    TString folderName;
    TString dirName;
    if(sourceDir.Contains("PseudoExperiment")){
      std::cout << "loading experiment from " << sourceDir << "/" << mlfitFile << std::endl;
      addExperimentFromRoofit(sourceDir+"/"+mlfitFile);
    }
    else{
      TSystemDirectory dir(sourceDir.Data(), sourceDir.Data());
      TList *folders = dir.GetListOfFiles();
      int counter = 1;
      //if folders are found, go through each one an look for the mlfitFile
      if (folders) {
        TIter next(folders);
        TStopwatch watch;
        while ((pseudoExperimentFolder=(TSystemFile*)next())) {
          watch.Start();
          folderName = pseudoExperimentFolder->GetName();
          if (pseudoExperimentFolder->IsFolder() && !folderName.EndsWith(".") && !folderName.Contains("asimov")) {
            if(debug_) std::cout << "DEBUG    ";
            if(debug_ || counter%10==0) std::cout << "Adding PseudoExperiment #" << counter << std::endl;
            addExperimentFromRoofit(sourceDir+"/"+folderName+"/"+mlfitFile);
            counter++;
          }
          watch.Stop();
          if(debug_) printTime(watch, "Time for last loop");
        }
        delete folders;
      }
    }
    if(pseudoExperimentFolder != NULL) delete pseudoExperimentFolder;
  }
  if(correlationsPostfitB_.size() == 0 && correlationvalsPostfitB_.size() != 0){
    convertToHisto(correlationvalsPostfitB_, correlationsPostfitB_, "Postfit B;Correlation; Frequency");
  }
  if(correlationsPostfitS_.size() == 0 && correlationvalsPostfitS_.size() != 0){
    convertToHisto(correlationvalsPostfitS_, correlationsPostfitS_, "Postfit S;Correlation; Frequency");
  }
  for(auto& container : prefitShapes_) container->createShapeHistos(injectedMu_);
  for(auto& container : postfitBshapes_) container->createShapeHistos(injectedMu_);
  for(auto& container : postfitSshapes_) container->createShapeHistos(injectedMu_);
}

bool PseudoExperiments::checkFitStatus(TFile* file){
  bool storeExperiment = true;
  int fit_status=7;
  TString fit_trees[2] = {"tree_fit_sb", "tree_fit_b"};
  bool fit_flags[2] = {fitSBmustConverge_, fitBmustConverge_};
  for(int nTrees=0; nTrees<2; nTrees++)
  {
    TTree* tree = (TTree*)file->Get(fit_trees[nTrees].Data());
    if(tree != NULL)
    {
      if(tree->SetBranchAddress("fit_status",&fit_status)>= 0)
      {
        tree->GetEntry(0);
        if((fit_status != 0)){
          std::cout << "WARNING fit_status in " << fit_trees[nTrees].Data() << " did not converge!\n";
          if(fit_flags[nTrees]) storeExperiment = false;
        }
      }
      delete tree;
      fit_status=7;
    }
    else{
      std::cerr << "ERROR   could not load tree " << fit_trees[nTrees].Data() << " from file " << file->GetName() << std::endl;
      storeExperiment = false;
    }
  }
  return storeExperiment;
}

bool PseudoExperiments::checkCovarianceMatrix(TFile* file){
  bool accurateCovariance = false;
  TString rooFitObjects[2] = {"fit_b", "fit_s"};
  int quality = -1;
  bool qualities[2] = {false, false};
  for(int currentObject=0; currentObject < 2; currentObject++){
    RooFitResult* fitObject = 0;
    file->GetObject(rooFitObjects[currentObject].Data(),fitObject);
    if( fitObject == 0 ) {
      std::cerr << "ERROR getting '" << rooFitObjects[currentObject].Data() << "' from file '" << file->GetName() << "'" << std::endl;
      //throw std::exception();
    }
    else{
      quality=-1;
      quality=fitObject->covQual();
      if(debug_)
      {
        std::cout << "    DEBUG: quality of covariance matrix in " << rooFitObjects[currentObject].Data() << " is " << quality;
        if(quality==-1) std::cout << rooFitObjects[currentObject].Data() << ": Unknown, matrix was externally provided\n";
        if(quality==0) std::cout << rooFitObjects[currentObject].Data() << ": Not calculated at all\n";
        if(quality==1) std::cout << rooFitObjects[currentObject].Data() << ": Approximation only, not accurate\n";
        if(quality==2) std::cout << rooFitObjects[currentObject].Data() << ": Full matrix, but forced positive-definite\n";
      }
      if(quality==3) {
        if(debug_) std::cout << rooFitObjects[currentObject].Data() << ": Full, accurate covariance matrix\n";
        qualities[currentObject] = true;
      }
    }
    delete fitObject;
  }
  if(!fitSBmustConverge_ && !fitBmustConverge_) return true;
  if(fitBmustConverge_){
    if(qualities[0]) accurateCovariance = true;
  }
  if(fitSBmustConverge_){
    if(qualities[1]) accurateCovariance = true;
    else accurateCovariance = false;
  }
  return accurateCovariance;
}

void PseudoExperiments::initContainers(TFile* file, const RooFitResult* result, int nBins, Double_t min, Double_t max) {
  
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  if(nps_.empty())
  {
    TIter it = result->floatParsFinal().createIterator();
    TString name;
    RooRealVar* param;
    while(( param = (RooRealVar*)it.Next() )){
        if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
        name = param->GetName();
        if( std::find(nps_.begin(), nps_.end(), name) == nps_.end() && !((name.Contains("prop_bin") || name.Contains("_Bin")) && noBinByBin_)){
            nps_.push_back(name);
        }
    }

    // while( varName.Contains(",") ) {
    //   npName = varName(varName.Last(',')+1, varName.Length() - varName.Last(','));
      
    //   if(!(npName.Contains("prop_bin") && noBinByBin_) && !npName.BeginsWith("r")) nps_.push_back(npName);
    //   varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
    // }
    // if(!(varName.Contains("prop_bin") && noBinByBin_) && !npName.BeginsWith("r")) nps_.push_back(varName);
  }

  createHistograms(valuesPrefit_,nps_,"prefit");
  createHistograms(valuesPrefiterrors_, nps_, "prefit_errors");
  createHistograms(valuesPrefiterrorHi_, nps_, "prefit_errorHi");
  createHistograms(valuesPrefiterrorLo_, nps_, "prefit_errorLo");
  createHistograms(valuesPostfitB_,nps_,"postfitB");
  createHistograms(valuesPostfitBerrors_, nps_, "postfitB_errors");
  createHistograms(valuesPostfitBerrorHi_, nps_, "postfitB_errorHi");
  createHistograms(valuesPostfitBerrorLo_, nps_, "postfitB_errorLo");
  createHistograms(valuesPostfitS_,nps_,"postfitS");
  createHistograms(valuesPostfitSerrors_,nps_,"postfitS_errors");
  createHistograms(valuesPostfitSerrorHi_, nps_, "postfitS_errorHi");
  createHistograms(valuesPostfitSerrorLo_, nps_, "postfitS_errorLo");
}

void PseudoExperiments::createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins, Double_t min, Double_t max) const {
  if (! nps.empty() )
  {
      for(auto& np: nps) {
        hists[np] = createHistogram(np, name, nBins, min, max);
      }
  }
}

TH1* PseudoExperiments::createHistogram(const TString& par, const TString& name, int nBins, Double_t min, Double_t max) const {
  if( debug_ ) std::cout << "DEBUG: createHistogram: " << par << ", " << name << std::endl;
  const TUUID id;
  TString clear_label = label_+":"+par+":"+name+":"+id.AsString();
  if(clear_label.Contains(" = ")) clear_label.ReplaceAll(" = ", "_");
  if(clear_label.Contains(" ")) clear_label.ReplaceAll(" ", "_");
  TH1* h = new TH1D(clear_label.Data(), name+";"+par+";N(experiments)", nBins, min, max);
  h->SetDirectory(0);
  return h;
}

void PseudoExperiments::storePrefitValues(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file) const {
  TStopwatch watch;
  if(debug_) std::cout << "loading fit_s for prefit values\n";
  RooFitResult* test_result = (RooFitResult*) file->Get("fit_s");
  //std::vector<TString> values;
  if(test_result != NULL){
    if(debug_) std::cout << "collecting variable names from RooFitResult fit_s" << std::endl;
    readRooRealVar(hists, hErrors, hErrorsHi, hErrorsLo, test_result->floatParsInit().createIterator());

    if(debug_)std::cout << "\ndone" << std::endl;
    //if(debug_) std::cout << "deleting test_result in storePrefitValues\n";
    if(test_result != NULL) delete test_result;
  }
}


void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, TH1*> >& correlations) {

  TStopwatch watch;
  if( nps_.empty() ) {
    if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
    initContainers(file, result);
  }
  else{
    if( pois_.empty()){
        if( debug_ ) std::cout << "  DEBUG: initialize POIs\n";
        TIter it = result->floatParsFinal().createIterator();
        TString name;
        RooRealVar* param;
        while(( param = (RooRealVar*)it.Next() )){
            if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
            name = param->GetName();
            if( std::find(nps_.begin(), nps_.end(), name) == nps_.end() && !((name.Contains("prop_bin") || name.Contains("_Bin")) && noBinByBin_)){
                pois_.push_back(name);
            }
        }
        createHistograms(valuesPrefit_,pois_,"prefit");
        createHistograms(valuesPrefiterrors_, pois_, "prefit_errors");
        createHistograms(valuesPrefiterrorHi_, pois_, "prefit_errorHi");
        createHistograms(valuesPrefiterrorLo_, pois_, "prefit_errorLo");
        createHistograms(valuesPostfitB_,pois_,"postfitB");
        createHistograms(valuesPostfitBerrors_, pois_, "postfitB_errors");
        createHistograms(valuesPostfitBerrorHi_, pois_, "postfitB_errorHi");
        createHistograms(valuesPostfitBerrorLo_, pois_, "postfitB_errorLo");
        createHistograms(valuesPostfitS_,pois_,"postfitS");
        createHistograms(valuesPostfitSerrors_,pois_,"postfitS_errors");
        createHistograms(valuesPostfitSerrorHi_, pois_, "postfitS_errorHi");
        createHistograms(valuesPostfitSerrorLo_, pois_, "postfitS_errorLo");
        createHistograms(biasPostfitS_, pois_, "biasfitS", 40, -2, 2);
    }
  }
  readRooRealVar(hists, hErrors, hErrorsHi, hErrorsLo, result->floatParsFinal().createIterator(), biasPostfitS_);

  if(debug_)std::cout << std::endl;
  collectCorrelations(correlations, result);
}

void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, std::vector<double> > >& correlations) {

  TStopwatch watch;
  if( nps_.empty() ) {
    if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
    initContainers(file, result);
  }
  else{
    if( pois_.empty()){
        if( debug_ ) std::cout << "  DEBUG: initialize POIs\n";
        TIter it = result->floatParsFinal().createIterator();
        TString name;
        RooRealVar* param;
        while(( param = (RooRealVar*)it.Next() )){
            if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
            name = param->GetName();
            if( std::find(nps_.begin(), nps_.end(), name) == nps_.end() && !((name.Contains("prop_bin") || name.Contains("_Bin")) && noBinByBin_)){
                pois_.push_back(name);
            }
        }
        createHistograms(valuesPrefit_,pois_,"prefit");
        createHistograms(valuesPrefiterrors_, pois_, "prefit_errors");
        createHistograms(valuesPrefiterrorHi_, pois_, "prefit_errorHi");
        createHistograms(valuesPrefiterrorLo_, pois_, "prefit_errorLo");
        createHistograms(valuesPostfitB_,pois_,"postfitB");
        createHistograms(valuesPostfitBerrors_, pois_, "postfitB_errors");
        createHistograms(valuesPostfitBerrorHi_, pois_, "postfitB_errorHi");
        createHistograms(valuesPostfitBerrorLo_, pois_, "postfitB_errorLo");
        createHistograms(valuesPostfitS_,pois_,"postfitS");
        createHistograms(valuesPostfitSerrors_,pois_,"postfitS_errors");
        createHistograms(valuesPostfitSerrorHi_, pois_, "postfitS_errorHi");
        createHistograms(valuesPostfitSerrorLo_, pois_, "postfitS_errorLo");

        createHistograms(biasPostfitS_, pois_, "biasfitS", 40, -2, 2);
    }
  }
  readRooRealVar(hists, hErrors, hErrorsHi, hErrorsLo, result->floatParsFinal().createIterator(), biasPostfitS_);

  if(debug_)std::cout << std::endl;
  collectCorrelations(correlations, result);
}


TH1* PseudoExperiments::getHist(const std::map<TString,TH1*>& hists, const TString& key) const {
  if(debug_) std::cout << "entering 'PseudoExperiments::getHist()'\n";
  TH1* returnPointer = NULL;
  std::map<TString,TH1*>::const_iterator it = hists.find(key);
  if( it == hists.end() ) {
    std::cerr << "ERROR trying to access histogram for '" << key << "' in '" << label_ << "'" << std::endl;
    //throw std::exception();
  }
  else returnPointer = it->second;
  if(debug_) std::cout << "leaving getHist()\n";
  return returnPointer;
}


TH1* PseudoExperiments::getClone(const TH1* h)const {
  if(debug_) std::cout << "entering PseudoExperiments::getClone()\n";
  if(h!=NULL){
    if(debug_) std::cout << "h exists!\n";
    const TUUID id;
    const TString name = h->GetName();
    return static_cast<TH1*>(h->Clone(name+":"+id.AsString()));
  }
  else std::cerr << "ERROR cloning histogram!\n";
  return NULL;
}

void PseudoExperiments::storeShapes(std::vector<ShapeContainer*>& shapes, TFile* file, const TString& name) const
{
  if(file->cd(name))
  {
    if(debug_) std::cout << "entering directory " << name.Data() << std::endl;
    TDirectory* shapeFolder = gDirectory;
    TIter nextFileObject(shapeFolder->GetListOfKeys());
    TKey* dirKey;
    bool createNewContainer = true;
    TString categoryName;
    TString signalStrength = label_;
    signalStrength.ReplaceAll(" ", "_");
    signalStrength.ReplaceAll("=", "");

    while ((dirKey = (TKey*)nextFileObject())) {
      if(dirKey->IsFolder())
      {
        categoryName = dirKey->GetName();
        if(debug_) std::cout << "current directory: " << categoryName << std::endl;
        createNewContainer = true;
        for(int nShapeContainer = 0; nShapeContainer<int(shapes.size()); nShapeContainer++)
        {
          if(shapes[nShapeContainer]->getName() == categoryName)
          {
            if(debug_) std::cout << "DEBUG    found container with matching category name!\n";
            shapes[nShapeContainer]->loadShapes(*file, name, signalStrength);
            createNewContainer = false;
          }
        }
        if(createNewContainer)
        {
          ShapeContainer* tempShapeContainer = new ShapeContainer(categoryName);
          tempShapeContainer->loadShapes(*file, name, signalStrength);
          shapes.push_back(tempShapeContainer);
        }
      }
    }
  }
  else std::cerr << "ERROR    Could not change into directory " << name.Data() << " in file " << file->GetName() <<std::endl;
}


TH2D* PseudoExperiments::getCorrelationPlot(const std::map<TString, std::map<TString,TH1*> >& correlations) const{
  int nNps = int(correlations.size());
  TH1* tempHisto = NULL;
  const TUUID id;
  TString name;
  name.Form("correlationPlot_%s", id.AsString());
  TH2D* correlationPlot = new TH2D(name.Data(),"", nNps, 0, nNps, nNps, 0, nNps);
  int i=1;
  int j=1;
  for(std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlations.begin(); it != correlations.end(); it++){

    correlationPlot->GetXaxis()->SetBinLabel(i, it->first);
    correlationPlot->GetYaxis()->SetBinLabel(i, it->first);
    j=i;
    for(std::map<TString, std::map<TString,TH1*> >::const_iterator sub_it = it; sub_it != correlations.end(); sub_it++){
      if(debug_) std::cout << "getting correlations for " << it->first << ", " << sub_it->first << std::endl;
      tempHisto = getHist(it->second, sub_it->first);
      if(tempHisto){
        if (debug_) std::cout << "setting bin content: (" << i << ", " << j << ") = " << tempHisto->GetMean() << std::endl;
        correlationPlot->SetBinContent(i,j, tempHisto->GetMean());
        correlationPlot->SetBinError(i,j, helperFuncs::checkValues(tempHisto->GetMeanError()));
        correlationPlot->SetBinContent(j,i, tempHisto->GetMean());
        correlationPlot->SetBinError(j,i, helperFuncs::checkValues(tempHisto->GetMeanError()));
        tempHisto = NULL;
      }
      else{
        correlationPlot->SetBinContent(i,j, -2);
        correlationPlot->SetBinContent(j,i, -2);
      }
      j++;
    }
    i++;
  }
  correlationPlot->GetZaxis()->SetRangeUser(-1, 1);
  return correlationPlot;
}

void PseudoExperiments::collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, const RooFitResult* result) {
  std::vector<TString> values;
  if(debug_) std::cout << "collecting correlations\n";

  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;

  TIter it = result->floatParsFinal().createIterator();
  TString name;
  RooRealVar* param;
  while(( param = (RooRealVar*)it.Next() )){
      if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
      name = param->GetName();
      values.push_back(name);
  }
  if(debug_)std::cout << "\ndone" << std::endl;
  if(debug_)std::cout << "current size of correlation container: " << correlations.size() << std::endl;
  if(correlations.empty()){
    if(debug_) std::cout << "initializing correlation histos\n";
    TString name;
    for(auto& varName : values){
      const TUUID id;
      name.Form("%s_correlations_%s", varName.Data(), id.AsString());
      createHistograms(correlations[varName], values, name, nBins_, -1.2, 1.2);
    }
  }
  for(auto& np_i : values){
    for(auto& np_j : values){
      correlations[np_i][np_j]->Fill(result->correlation(np_i, np_j));
    }
  }
}

void PseudoExperiments::collectCorrelations(std::map<TString, std::map<TString,std::vector<double>> >& correlations, const RooFitResult* result) {
  std::vector<TString> values;
  if(debug_) std::cout << "collecting correlations\n";

  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  TIter it = result->floatParsFinal().createIterator();
  TString name;
  RooRealVar* param;
  while(( param = (RooRealVar*)it.Next() )){
      if(!(helperFuncs::compare_classname(param, "RooRealVar"))) continue;
      name = param->GetName();
      if(!((name.Contains("prop_bin") || name.Contains("_Bin")) && noBinByBin_)) values.push_back(name);
  }
  // while( varName.Contains(",") ) {
  //   paramName = varName(varName.Last(',')+1, varName.Length() - varName.Last(','));
  //   if(debug_) std::cout << paramName << std::endl;
  //   if(!(paramName.Contains("prop_bin") && noBinByBin_)) 
  //   {
  //     if(debug_)std::cout << "saving " << paramName << std::endl;
  //     values.push_back(paramName);
  //   }
  //   varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
  // }
  // if(!(varName.Contains("prop_bin") && noBinByBin_)) values.push_back(varName);
  if(debug_)std::cout << "\ndone" << std::endl;
  if(debug_)std::cout << "current size of correlation container: " << correlations.size() << std::endl;

  for(auto& np_i : values){
    for(auto& np_j : values){
      if(debug_) std::cout << "getting correlation for " << np_i << "\t" << np_j << std::endl;
      correlations[np_i][np_j].push_back(result->correlation(np_i, np_j));
    }
  }
}

void PseudoExperiments::printCorrelations(const std::map<TString, std::map<TString,TH1*> >& correlations, const TString& outpath) const {
  TString currentName;
  for(std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlations.begin(); it != correlations.end(); it++){
    for(std::map<TString,TH1*>::const_iterator sub_it = it->second.begin(); sub_it != it->second.end(); sub_it++){
      currentName.Form("correlation_%s_%s_%s",it->first.Data(), sub_it->first.Data(), label_.Data());
      currentName.ReplaceAll("=", "_");
      currentName.ReplaceAll(" ", "_");
      currentName.ReplaceAll(".", "p");
      currentName.Prepend(outpath);
      currentName.Append(".root");
      TFile* outfile = TFile::Open(currentName.Data(), "RECREATE");
      TCanvas canvas;
      sub_it->second->Draw();
      sub_it->second->Write();
      canvas.Write("canvas");
      currentName.ReplaceAll(".root", ".pdf");
      canvas.SaveAs(currentName.Data());
      outfile->Close();
    }
  }
}