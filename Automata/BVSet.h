// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BVSet.h
 * Definitions for a set of BitVectors
 */
#ifndef BVSET_H_
#define BVSET_H_
#include "BitVector.h"

struct ltstruct {
  bool operator()(const class BitVector &s1, const class BitVector &s2) const
  {
    assert(s1.getSize()==s2.getSize());
    for(unsigned i=s1.getSize(); i--; ) {
      if(s1[i] < s2[i] ) return true;
      else if (s1[i] > s2[i]) return false; 
    }
    return false;
  }
};

#include <set>
typedef std::set<class BitVector, ltstruct> BVSet;

#endif //BVSET_H_
