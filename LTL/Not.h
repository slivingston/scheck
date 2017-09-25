// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Not.h
 * Class for representing the negation of a formula
 */

#ifndef NOT_H_
#define NOT_H_
#ifdef __GNUC__
#pragma interface
#endif // __GNUC__

#include "Formula.h"

class Not : public Formula {

 public:
  /**Construct a negated formula
   *@param formula Formula to be negated
   */
  Not(const class Formula *formula);
  /**The destructor*/
  ~Not() {; }
  void destroy() {
    const_cast<Formula *>(myFormula)->destroy();
    delete this;
  }
  /**The copy constructor*/
  Not(const class Not &other);
  /**@return A deep copy of this object*/
  Not* clone() const {
    return new Not(*this);
  }
  /**The assignment operator*/
  class Not & operator=(const class Not &other);
  /**@return The operand*/
  const class Formula *getOperand() const {return  myFormula;}
  class Formula *getOperand() {return const_cast<Formula *>(myFormula);}
  
  /**Print the formula in post-fix order to file*/
  void print (FILE *file) const;
  /**@return The type of formula*/
  enum Formula::Type getType() const { return fNot;} 
  /**@return hashvalue of the formula*/
  int hash() const {return 0x9e3779b9 ^ myFormula->hash();}
  /**@return true if the other formula is syntactically equal*/
  bool operator==(const class Formula &other) const;
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  bool eval(const NumberMap & apmap, unsigned ap) const;


 private:
  //the negated formula
  const class Formula *myFormula;  
};

#endif //NOT_H_
