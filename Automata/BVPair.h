// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BVPair.h
 * Definitions for a set of pairs of BitVectors and unsigned ints
 */
#ifndef BVPAIR_H_
#define BVPAIR_H_

#include "ExtHashSet.h"

typedef std::pair<class BitVector, unsigned> Pair;
struct eqpair {
  bool operator()(const Pair &p1, const Pair &p2) const {
    return p1.first==p2.first && p1.second==p2.second;
  }
};
struct phasher 
{
  unsigned operator()(const Pair &s) const
  {
    unsigned hvalue=246093032;
    hvalue ^= s.first.hash(hvalue);
    return hvalue^(s.second);
  }
};
typedef Sgi::hash_set<Pair, phasher, eqpair> BVPair;


#endif //BVPAIR_H_
