// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file TemporalBinOp.h
 * Class for representing the Temporal Binary operators 
 */

#ifndef TEMPORALBINOP_H_
#define TEMPORALBINOP_H_
#ifdef __GNUC__
#pragma interface
#endif //__GNUC__
#include "Formula.h"

class TemporalBinOp: public Formula {
  
public:
  enum Op {Until, Release};
  
  /**Constructor of the class
   * @param op The applied operator
   * @param left the left operand of the formula
   * @param rigth The right operand of the formula
   */
  TemporalBinOp(Op op, const class Formula *left, const class Formula *right);
  /**The destructor*/
  ~TemporalBinOp() { ;}
  /**Deallocate this object and all subformulae */
  void destroy() {
    const_cast<Formula *>(myLeft)->destroy();
    const_cast<Formula*>(myRight)->destroy();
    delete this;
  }

  /**@return The LHS formula*/
  const class Formula *getLHS() const {return myLeft;}
  class Formula *getLHS() { return const_cast<class Formula *>(myLeft);}
  /**@return The RHS formula*/
  const class Formula *getRHS() const {return myRight;}
  class Formula *getRHS() {return const_cast<class Formula *>(myRight);}
  /**The copy constructor*/
  TemporalBinOp(const class TemporalBinOp &other);
  /**The assignment operator*/
  class TemporalBinOp & operator=(const class TemporalBinOp &other);
  /**@return A deep copied clone of this object*/
  TemporalBinOp * clone() const {
    return new TemporalBinOp(*this);
  }
  /**Print the formula in post-fix order to file*/
  void print(FILE *file) const;
  /**@return The type of formula*/
  enum Formula::Type getType() const { return fTemporalBinOp;}
  /**@return The hashvalue of the formula*/
  int hash() const {
    return myOp*myLeft->hash() + myRight->hash();
  }
  /**@return true of the Formulas are syntactically equal*/
  bool operator==(const class Formula &other) const;
  /**@return The operation of the class*/
  Op getOp() const {return myOp;}
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  bool eval(const NumberMap & apmap, unsigned ap) const {
    return false;
  }
  
private:
  /**The operation*/
  enum Op myOp;
  /**The left-hand operand*/ 
  const class Formula  *myLeft;
  /**The right-hand operand*/
  const class Formula *myRight;   
};
#endif //TEMPORALBINOP_H_
