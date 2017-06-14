#ifndef CATEGORY_H
#define CATEGORY_H

#include "TString.h"

class Category {
public: 
  enum Type {
    SL_43, SL_43l, SL_43h,
    SL_44, SL_44l, SL_44h,
    SL_53, SL_53l, SL_53h,
    SL_54, SL_54l, SL_54h,
    SL_63, SL_63l, SL_63h,
    SL_64, SL_64l, SL_64h,
  };

  static TString toString(const Type category);
};


TString Category::toString(const Category::Type category) {
  if( category == SL_43 ) return "ljets_j4_t3";
  if( category == SL_44 ) return "ljets_j4_t4";
  if( category == SL_53 ) return "ljets_j5_t3";
  if( category == SL_54 ) return "ljets_j5_tge4";
  if( category == SL_63 ) return "ljets_jge6_t3";
  if( category == SL_64 ) return "ljets_jge6_tge4";

  if( category == SL_43l ) return "ljets_j4_t3_low";
  if( category == SL_44l ) return "ljets_j4_t4_low";
  if( category == SL_53l ) return "ljets_j5_t3_low";
  if( category == SL_54l ) return "ljets_j5_tge4_low";
  if( category == SL_63l ) return "ljets_jge6_t3_low";
  if( category == SL_64l ) return "ljets_jge6_tge4_low";

  if( category == SL_43h ) return "ljets_j4_t3_high";
  if( category == SL_44h ) return "ljets_j4_t4_high";
  if( category == SL_53h ) return "ljets_j5_t3_high";
  if( category == SL_54h ) return "ljets_j5_tge4_high";
  if( category == SL_63h ) return "ljets_jge6_t3_high";
  if( category == SL_64h ) return "ljets_jge6_tge4_high";

  return "UNDEFINED";
}
#endif
