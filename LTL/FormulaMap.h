// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file FormulaMap.h
 * Define mapping from formulas to numbers 
 */
#ifndef FORMULAMAP_H_
#define FORMULAMAP_H
#include "ExtHashMap.h"
#include "Formula.h"


struct fHasher
{
  size_t operator()(const Formula *f) const
  {  
        
    return f->hash();
    //return hashformula(*f)  
/*     unsigned key = (unsigned) f; */
/*     key += key << 12; */
/*     key ^= key >> 22; */
/*     key += key << 4; */
/*     key ^= key >> 9; */
/*     key += key << 10; */
/*     key ^= key >> 2; */
/*     key += key << 7; */
/*     key ^= key >> 12; */
/*     return key; */
  }

};

struct fMapEq {
  bool operator() (const Formula * f1, const Formula *f2) const {
    if (f1 == f2)
      return true;
    else
      return *f1 == *f2;
  }
};

typedef Sgi::hash_map<const Formula *, unsigned, fHasher, fMapEq> FormulaMap;

#endif //FORMULAMAP_H_
