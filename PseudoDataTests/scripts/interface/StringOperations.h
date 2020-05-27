#ifndef STRING_OPERATIONS_H
#define STRING_OPERATIONS_H
 
#include <string>
#include <vector>
 
#include "TString.h"
 
 
// Tools to modify / parse strings
namespace str {
  std::vector<TString> split(const TString& str, const TString& delim);
  std::vector<std::string> split(const std::string& str, const std::string& delim);
  bool split(const TString& str, const TString& delim, TString& first, TString& second);
  bool split(const std::string& str, const std::string& delim, std::string& first, std::string& second);
  TString after(const TString& str, const TString& delim);
  bool enclosed(const TString& str, const TString& delimStart, const TString& delimEnd, TString& encl);
  void trim(std::string& str);
  TString fileName(const TString& path);
}

// ----------------------------------------------------------------------------
std::vector<TString> str::split(const TString& str, const TString& delim) {
  std::vector<TString> parts;
  TString rest = str;
  TString part1 = "";
  TString part2 = "";
  while( split(rest,delim,part1,part2) ) {
    parts.push_back(part1);
    rest = part2;
  }
  parts.push_back(rest);
 
  return parts;
}
 
 
// ----------------------------------------------------------------------------
std::vector<std::string> str::split(const std::string& str, const std::string& delim) {
  std::vector<std::string> parts;
  std::vector<TString> partsTmp = split(TString(str.c_str()),TString(delim.c_str()));
  for(std::vector<TString>::const_iterator it = partsTmp.begin();
      it != partsTmp.end(); ++it) {
    parts.push_back(std::string(it->Data()));
  }
 
  return parts;
}
 
 
// ----------------------------------------------------------------------------
bool str::split(const TString& str, const TString& delim, TString& first, TString& second) {
  std::string strt(str.Data());
  std::string delimt(delim.Data());
  std::string firstt(first.Data());
  std::string secondt(second.Data());
  bool result = split(strt,delimt,firstt,secondt);
  first = firstt.c_str();
  second = secondt.c_str();
 
  return result;
}
 
 
// ----------------------------------------------------------------------------
bool str::split(const std::string& str, const std::string& delim, std::string& first, std::string& second) {
  bool hasDelim = false;
  first = str;
  second = "";
  if( str.find(delim) != std::string::npos ) {
    hasDelim = true;
    size_t end = str.find(delim);
    first = str.substr(0,end);
    second = str.substr(end+delim.size(),str.size());
  }
  trim(first);
  trim(second);
 
  return hasDelim;
}
 
 
// ----------------------------------------------------------------------------
TString str::after(const TString& str, const TString& delim) {
  TString first = "";
  TString second = "";
  split(str,delim,first,second);
 
  return second;
}
 
 
// ----------------------------------------------------------------------------
bool str::enclosed(const TString& str, const TString& delimStart, const TString& delimEnd, TString& encl) {
  bool hasDelims = false;
  std::string strt(str.Data());
  std::string enclt;
  size_t start = strt.find(delimStart.Data());
  size_t end = strt.find(delimEnd.Data());
  if( start != std::string::npos && end != std::string::npos && end > start ) {
    hasDelims = true;
    enclt = strt.substr(start+delimStart.Length(),end-start-1);
  }
  trim(enclt);
  encl = enclt;
 
  return hasDelims;
}
 
 
 
// ----------------------------------------------------------------------------
void str::trim(std::string& str) {
  while( str.size() && str[0] == ' ' ) str.erase(0,1);
  while( str.size() && str[str.size()-1] == ' ' ) str.erase(str.size()-1,str.size());    
}


// ----------------------------------------------------------------------------
TString str::fileName(const TString& path) {
  std::vector<TString> parts = split(path,"/");
  return parts.back();
}

#endif
