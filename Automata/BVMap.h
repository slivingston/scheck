// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BVMap.h
 * Definitions for mapping BitVectors to unsigned
 */
#ifndef BVMAP_H_
#define BVMAP_H_
#include "ExtHashMap.h"
#include "BitVector.h"

struct bveq 
{
  bool operator() (const class BitVector &s1, const class BitVector &s2) const
  {
    return s1==s2;
  }
};

struct bvhasher
{
  int operator()(const BitVector &s) const
  {
    return s.hash();
  }
};

typedef Sgi::hash_map<class BitVector, unsigned, bvhasher, bveq> BVMap;


#endif //BVMAP_H_
