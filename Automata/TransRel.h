// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @TransRel.h
 * Define a transition relation using the stl multimap
 */

#ifndef TRANSREL_H_
#define TRANSREL_H_
#ifdef SGI_HASH_MAP
#include "ExtHashMap.h"
#else 
#include <map>
#endif //SGI_HASH_MAP

class UIPair {
 public:
  explicit UIPair(unsigned s=0, unsigned l=0) : state(s), letter(l) {;}
  UIPair(const class UIPair &old) : state(old.state), letter(old.letter) {;}
  class UIPair & operator=(const class UIPair &rhs) {
    if (&rhs == this) return *this;
    state=rhs.state;
    letter=rhs.letter;
    return *this;
  }
  bool operator==(const class UIPair &rhs) const {
    return (state==rhs.state && letter == rhs.letter);
  }
  bool operator<(const class UIPair &rhs) const {
    if(rhs.state > state) 
      return true;
    if(rhs.state < state)
      return false;
    return (letter < rhs.letter);
  }
  unsigned state; 
  unsigned letter;
#ifdef SGI_HASH_MAP
  unsigned operator () (const class UIPair &p) const {
    return hasher(p.state)^hasher(p.letter);
  }
 private:
  /** Integer hash function by Robert Jenkins (bob_jenkins@compuserve.com)
   * <URL:http://ourworld.compuserve.com/homepages/bob_jenkins/blockcip.htm>
   * @param key       key to be hashed
   * @return          hash value of key
   */
  unsigned hasher (unsigned key) const {
    key += key << 12;
    key ^= key >> 22;
    key += key << 4;
    key ^= key >> 9;
    key += key << 10;
    key ^= key >> 2;
    key += key << 7;
    key ^= key >> 12;
    return key;
  }
#endif //SGI_HASH_MAP
};


#ifndef SGI_HASH_MAP 
struct lt {
  bool operator()(const class UIPair &p1, const class UIPair &p2) const {
    return (p1 < p2);
  }
};
#endif //SGI_HASH_MAP


#ifdef SGI_HASH_MAP
typedef Sgi::hash_multimap<class UIPair, unsigned, class UIPair> TransRel;
#else
typedef std::multimap<class UIPair, unsigned, lt> TransRel;
#endif //SGI_HASH_MAP

#endif //TRANSREL_H_
