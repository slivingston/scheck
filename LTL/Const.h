// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Const.h
 * Class for representing propositional constants true, false
 */
#ifndef CONST_H_
#define CONST_H_
#ifdef __GNUC__
#pragma interface
#endif //__GNUC__
#include "Formula.h"

class Const : public Formula {
 public:
  /**Construct a new constant
   *@param value Value of the constant
   */
  Const(bool value);
  /**The destructor*/
  ~Const() {; }
  void destroy() {
    delete this;
  }
  /**The copy constructor*/
  Const(const class Const &other);
  /**@return A deep copy of this object*/
  class Const* clone() const {
    return new Const(myValue);
  }
  /**The assignment operator*/
  class Const & operator=(const class Const &other);
  /**@return the value*/
  bool getVal() const {return myValue;}
  /**Print the formula in post-fix order to file*/
  void print(FILE *file) const;
    /**@return The type of formula*/
  enum Formula::Type getType() const { return fConst;} 
  /**@return The hashvalue of the formula*/  
  int hash() const {return myValue ? 2 : 1;}
  /**@return true if the formulas are syntactically equal*/  
  bool operator==(const class Formula &other) const;
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  bool eval(const NumberMap & apmap, unsigned ap) const {
    return myValue;
  }

 private:
  bool myValue;
};
#endif //CONST_H_
