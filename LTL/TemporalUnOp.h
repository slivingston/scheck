// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file TemporalUnOp.h
 * Class for representing the unary Temporal operators 
 */

#ifndef TEMPORALUNOP_H_
#define TEMPORALUNOP_H_
#ifdef __GNUC__
#pragma interface
#endif // __GNUC__
#include "Formula.h"

class TemporalUnOp: public Formula {
  
 public:
  enum Op {Finally, Globally, Next};

  /**Construct a new formula
   *@param op Which operator to apply
   *@param formula The formula to apply the operator on
   */
  TemporalUnOp(TemporalUnOp::Op op, const class Formula *formula);
  /**The destructor*/
  ~TemporalUnOp() {; }
  /**Deallocate this object and all subformulae */
  void destroy() {
    const_cast<Formula *>(myFormula)->destroy();
    delete this;
  }
  /**The copy constructor*/
  TemporalUnOp(const class TemporalUnOp &other);
  /**The assignment operator*/
  class TemporalUnOp & operator=(const class TemporalUnOp &other);
  TemporalUnOp* clone() const {
    return new TemporalUnOp(*this);
  }
  /**@return The operand*/
  const class Formula *getOperand() const {return myFormula;}
  class Formula *getOperand() {return const_cast<class Formula *>(myFormula);}
  /**Print the formula in post-fix order to file*/  
  void print(FILE *file) const;
    /**@return The type of formula*/
  enum Formula::Type getType() const { return fTemporalUnOp;}  
  /**@return The hashvalue of the formula*/
  int hash() const {return myOp^myFormula->hash();}
  /**@return true of the Formulas are syntactically equal*/
  bool operator==(const class Formula &other) const;
  /**@return The operation of the class*/
  Op getOp() const {return myOp;}
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  bool eval(const NumberMap &apmap, unsigned ap) const {
    return false;
  }
  

 private:
  enum Op myOp;   
  const class Formula *myFormula;
};

#endif //TEMPORALUNOP_H_
