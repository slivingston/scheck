// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BinOp.h
 * Class for representing Binary propositional operators 
 */

#ifndef BINOP_H_
#define BINOP_H_
#ifdef __GNUC__
#pragma interface
#endif //__GNUC__
#include "Formula.h"

class BinOp: public Formula {
  
 public:

  enum Op {And, Or, Impl, Equiv};

  /**Construct a new BinOp
   *@param op The connective
   *@param left The left operand
   *@param right The right operand
   */
  BinOp(BinOp::Op op, const class Formula *left, const class Formula *right);
  /**The destructor*/
  ~BinOp() { ;}
  /**Deallocate this object and all subformulae*/
  void destroy() {
    const_cast<Formula *>(myLeft)->destroy();
    const_cast<Formula *>(myRight)->destroy();
    delete this;
  }
  /**The copy constructor*/
  BinOp(const class BinOp &other);
  /**The assignment operator*/
  class BinOp & operator=(const class BinOp &other);
  BinOp* clone() const {
    return new BinOp(*this);
  }
  /**@return The LHS formula*/
  const class Formula *getLHS() const {return myLeft;}
  class Formula *getLHS() {return const_cast<class Formula *>(myLeft);}
  /**@return The RHS formula*/
  const class Formula *getRHS() const {return myRight;}
  class Formula *getRHS() {return const_cast<class Formula *>(myRight);}
  /**Print the formula in post fix-order to file*/
  void print(FILE *file) const;
  /**@return The type of formula*/
  enum Formula::Type getType() const { return fBinOp;} 
  /**@return The hashvalue of the formula*/
  int hash() const {
    return 0x9e3779b9*myLeft->hash() + myOp^myRight->hash();
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
  bool eval(const NumberMap & apmap, unsigned ap) const;

 private:
  /**The operation*/
  enum Op myOp;
  /**The left-hand operand*/
  const class Formula *myLeft;
  /**The right-hand operand*/
  const class Formula *myRight;
};
#endif //BINOP_H_
