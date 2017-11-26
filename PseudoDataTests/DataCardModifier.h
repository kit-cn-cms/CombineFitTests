#ifndef DATA_CARD_MODIFIER_H
#define DATA_CARD_MODIFIER_H

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TString.h"

#include "HistReader.h"
#include "StringOperations.h"


class DataCardModifier {
public:
  DataCardModifier(const TString& fileName);

  void setInputsPath(const TString& path=".");
  void setObservation(const TString& fileNameDataObs, const TString& fileNameDataObsInCard="");
  void write(const TString& outName) const;
  
private:
  std::vector<TString> lines_;
  int imax_;
  bool debug_;
};


DataCardModifier::DataCardModifier(const TString& fileName) {
  lines_.clear();
  imax_ = -1;
  debug_ = false;

  std::ifstream file;
  file.open(fileName.Data());
  if( file.is_open() && file.good() ) {
    if( debug_ ) std::cout << "DataCard: reading " << fileName << std::endl;
    std::string line;
    while( std::getline(file,line) ) {
      TString lineTmp(line.c_str());
      if( lineTmp.BeginsWith("imax") ) {
	std::vector<TString> words = str::split(lineTmp," ");
	const TString imax = words.at(1);
	if( imax.IsDigit() ) {
	  imax_ = imax.Atoi();
	  if( debug_ ) std::cout << "DataCard: found imax = " << imax_ << std::endl;
	}
      }
      lines_.push_back( TString(line.c_str()) );
    }
    file.close();
  } else {
    std::cerr << "ERROR: unable to read datacard '" << fileName << "'" << std::endl;
    throw std::exception();
  }
}


void DataCardModifier::setInputsPath(const TString& path) {
  std::vector<TString> newLines;

  for(auto& line: lines_) {
    if( line.BeginsWith("shapes ") ) {
      const std::vector<TString> words = str::split(line," ");
      const TString oldInputFull = words.at(3); // looks like 'some/path/file.root'
      const TString fileName = oldInputFull.Contains("/") ? str::split(oldInputFull,"/").back() : oldInputFull;
      if( !fileName.EndsWith(".root") ) {
	std::cerr << "ERROR DataCard: input file not found" << std::endl;
	std::cerr << "  in file '" << fileName << "'" << std::endl;
	std::cerr << "  in line '" << line << "'" << std::endl;	
	throw std::exception();
      }
      TString newLine = "";
      for(size_t i = 0; i < words.size(); ++i) {
	if( i == 3 ) newLine += path+"/"+fileName+" ";
	else         newLine += words.at(i)+" ";
      }
      newLines.push_back(newLine);

    } else {
      newLines.push_back(line);
    }
  }

  lines_ = newLines;
}


void DataCardModifier::setObservation(const TString& fileNameDataObs, const TString& fileNameDataObsInCard) {
  std::vector<TString> newLines;

  HistReader reader;
  std::vector<double> yields;
  for(auto& line: lines_) {
    // ignore lines defining data_obs processes (we want to replace them)
    if( line.BeginsWith("shapes data_obs") ) continue;
    
    if( line.BeginsWith("shapes *") ) {
      std::vector<TString> words = str::split(line," ");
      const TString channel = words.at(2);
      const TString nominal = words.at(4).ReplaceAll("$PROCESS","data_obs");;
      const TString systematic = words.at(5).ReplaceAll("$PROCESS","data_obs");;

      const TString dataObsInCard = fileNameDataObsInCard!="" ? fileNameDataObsInCard : fileNameDataObs;
      const TString dataLine = "shapes data_obs "+channel+" "+dataObsInCard+" "+nominal+" "+systematic;
      newLines.push_back(line);
      newLines.push_back(dataLine);

      TH1* h = reader(fileNameDataObs,nominal);
      yields.push_back(h->Integral());
      delete h;

    } else {
      newLines.push_back(line);
    }
  }

  if( imax_ > 0 ) {
    if( imax_ != static_cast<int>(yields.size()) ) {
      std::cerr << "ERROR DataCard: number of replaced data_obs(" << yields.size() << ") does not match imax(" << imax_ << ")" << std::endl;
      throw std::exception();
    }
  }
  
  for(auto& line: newLines) {
    if( line.BeginsWith("observation") ) {
      TString newObservation = "observation";
      for(auto& yield: yields) {
	newObservation += " ";
	newObservation += yield;
      }
      line = newObservation;
    }
  }

  lines_ = newLines;
}


void DataCardModifier::write(const TString& outName) const {
  std::ofstream out(outName.Data());
  if( out.is_open() ) {
    for(auto& line: lines_) {
      out << line << std::endl;
    }
    out.close();
  } else {
    std::cerr << "ERROR: unable to open '" << outName << "' for writing" << std::endl;
    throw std::exception();
  }
}

#endif
