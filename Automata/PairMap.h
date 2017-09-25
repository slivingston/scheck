// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file PairMap.h
 * Map a pair of unsigned integers to an integer
 */

#ifndef PAIRMAP_H_
#define PAIRMAP_H_
#ifdef __GNUC__
#pragma interface
#endif // __GNUC__

#include <cassert>
class PairMap {
 public:
  /**Constructor of the class
   *@param size Number of pairs to map
   */
  explicit PairMap(unsigned xsize=1, unsigned ysize=1);
  /**The destructor*/
  ~PairMap();
 private:
  /**Copy constructor*/
  PairMap(const class PairMap &old);
  /**Assignment operator*/
  class PairMap & operator=(const class PairMap &rhs);
 public:
  /**Extend the map
   * @param xsize The new xsize of the map
   * @param ysize The new ysize of the map
   */
  void grow(unsigned xsize, unsigned ysize);
  /**Get an element 
   * @param state
   * @param label
   * @return the element
   */
  unsigned get(unsigned state, unsigned label) const {
    assert(state<myXSize && label<myYSize);
    return myMap[state][label]-1;
  }
  /**Set an element 
   * @param state
   * @param label
   * @param dest
   */
  void set(unsigned state, unsigned label, unsigned dest) {
    assert(state<myXSize && label<myYSize);
    myMap[state][label]=++dest;
    return;
  }
  /**Clear a slot 
   *@param state
   *@param label
   */
  void clear(unsigned state, unsigned label) {
    assert(state<myXSize && label < myYSize);
    myMap[state][label]=0;
    return;
  }
  /**Check if a slot is empty
   * @param state 
   * @param label 
   * @return true iff the slot is empty
   */
  bool isEmpty(unsigned state, unsigned label) const {
    assert(state<myXSize && label<myYSize);
    return myMap[state][label]==0;
  }
  /**@return the number of states*/
  unsigned numStates() const {return myXSize;}
  /**@return the alphabet size*/
  unsigned numAlphbet() const {return myYSize;}

 private:
  unsigned myXSize;
  unsigned myYSize;
  unsigned myXAllocated;
  unsigned myYAllocated;
  unsigned **myMap;

};

#endif //PAIRMAP_H_
