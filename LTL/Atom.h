// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Atom.h
 * Class for representing atomic propositions 
 */
#ifndef ATOM_H_
#define ATOM_H_
#ifdef __GNUC__
#pragma interface
#endif //__GNUC__
#include <cassert>
#include "Formula.h"

class Atom : public Formula {
  
 public: 
  /**Constructor
   *@param number Id for atom
   */
  Atom(unsigned number, bool sign = true);
  /**The destructor*/
  ~Atom() {; }
  void destroy() {
    delete this;
  }
  /**The copy constructor*/
  Atom(const class Atom &other);
  /**The assignment operator*/
  class Atom & operator=(const class Atom &other);
  /**@return A deep copy of this object*/
  class Atom* clone() const {
    return new Atom(myId, mySign);
  }
  /**@return the id*/
  unsigned getId() const {return myId;}
  /**@return The sign of the ap*/
  bool getSign() const {return mySign;}
  /**Print the formula in post-fix order to file*/
  void print(FILE *file) const;
  /**@return The type of formula*/
  enum Formula::Type getType() const { return fAtom;}
  /**@return The hashvalue of the formula*/
  int hash() const {return 0x9e3779b9 + myId;}
  /**@return true of the Formulas are syntactically equal*/
  bool operator==(const class Formula &other) const;
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  bool eval(const NumberMap & apmap, unsigned ap) const {
    assert(apmap.find(myId)!=apmap.end());
    unsigned mask= 1<< (*apmap.find(myId)).second;
    return ((mask & ap) > 0);
  }
  
 private:

  /**id number for atom*/
  unsigned myId;
  bool mySign;
  
};

#endif //ATOM_H_
