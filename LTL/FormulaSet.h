// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file FormulaSet.h
 * Define a set of formulas
 */
#ifndef FORMULASET_H_
#define FORMULASET_H
#include "Formula.h"
#ifdef SGI_HASH_MAP
#include "ExtHashSet.h"
struct fHasher
{
  size_t operator()(const Formula *f) const
  {  
    return f->hash();
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
typedef Sgi::hash_set<Formula *, fHasher, fMapEq> FormulaSet;
#endif //SGI_HASH_MAP
#endif //FORMULASET_H_
