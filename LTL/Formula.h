// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Formula.h
 * Abstract Base class for representing LTL formulae 
 */

#ifndef FORMULA_H_
#define FORMULA_H_
#ifdef __GNUC__
# pragma interface
#endif //__GNUC__
#include <stack>
#include "NumberMap.h"

//forward declaration of file
struct _IO_FILE;
typedef struct _IO_FILE FILE;

class Formula {  
public: 
  typedef std::stack<const Formula*> FormulaStack;
  //forward definition of Iterator
  class Iterator;
  class PostIterator;
  /**Different types of expressions*/
  enum Type { fTemporalBinOp, fTemporalUnOp, fBinOp, 
	      fAtom, fConst, fNot
  }; 
  /**Construct a new Formula*/
  Formula() : myNum(0) {;}
  /**Print the formula in post-fix order to file*/
  virtual void print(FILE *file) const = 0;
  /**@return The type of expression*/
  virtual enum Formula::Type getType() const=0;
  /**@return A deep copied clone of this object*/
  virtual Formula * clone() const = 0;
  /**The destructor of the class, deletes only this object*/
  virtual ~Formula() {;}
  /**Deallocate this object and all sub formulas too*/
  virtual void destroy() {;} 
  /**@return The hashvalue of the formula*/
  virtual int hash() const = 0;
  /**@return true of the Formulas are syntactically equal*/
  virtual bool operator==(const class Formula &other) const=0;
  /**@return An Iterator to the formula*/
  class Iterator newIterator() const;
  class PostIterator newPostIterator() const;  
  /**Evaluate the formula with the given bindings (only for propositional formulas)
   * @param apmap mapping from ap_id to ap_num
   * @param ap mapping from ap_num to {true, false}
   * @return true iff the formula evaluates to true
   */
  virtual bool eval(const NumberMap & apmap, unsigned ap) const = 0;
  /**Get/set ordering number for the formula*/
  unsigned getNum() const {return myNum;}
  void setNum(unsigned num) {myNum=num;} 
  
protected:
  /**The copy constructor*/
  Formula(const Formula &other);
  /**Assigment operator*/
  class Formula & operator=(const Formula &other);
  /**Ordering number for the formula*/
  unsigned myNum;
};



/**Traverse the formula parse tree in preorder*/
class Formula::Iterator {
public:
  /**Constructor of the class
   *@param formula Formula to iterate trough
   */
  //Iterator(const class Formula *formula);
  Iterator(const class Formula *formula): myStack(*new class FormulaStack()) 
  {
    myStack.push(formula);
  }
  /**Destructor*/
  ~Iterator();
  /**Advance to the next subformula*/
  void operator++();
  /**@return current subformula*/
  class Formula & operator*();
  /**const version of the method above*/
  const class Formula & operator*() const;
  /**@return false if there are untraversed subformulas*/
  bool atEnd() const;

private:
  class FormulaStack &myStack;
};

/**Traverse the formula in postorder*/
class Formula::PostIterator {
public:
  /**Constructor of the class
   *@param formula Formula to iterate trough
   */
  PostIterator(const class Formula *formula);
  /**Destructor*/
  ~PostIterator();
  /**Advance to the next subformula*/
  void operator++();
  /**@return current subformula*/
  class Formula & operator*();
  /**const version of the method above*/
  const class Formula & operator*() const;
  /**@return false if there are untraversed subformulas*/
  bool atEnd() const;

private:
  class FormulaStack &myStack;
  class Formula *myCurrent;
};

/**Inline function definitions*/


inline Formula::Iterator 
Formula::newIterator() const
{
  return Iterator(this); //return by value to avoid dangling refrence
}


inline Formula::PostIterator 
Formula::newPostIterator() const
{
  return PostIterator(this); //return by value to avoid dangling refrence
}

inline const class Formula &
Formula::Iterator::operator*() const 
{
  return *myStack.top();
}

inline class Formula &
Formula::Iterator::operator*() 
{
  return const_cast<Formula &>(*myStack.top());
}

inline bool
Formula::Iterator::atEnd() const
{
  return myStack.empty();
}

inline
Formula::Iterator::~Iterator()
{
  delete &myStack;
}

inline const class Formula &
Formula::PostIterator::operator*() const 
{
  return *myCurrent;
}

inline class Formula &
Formula::PostIterator::operator*() 
{
  return *myCurrent;
}

inline bool
Formula::PostIterator::atEnd() const
{
  return myCurrent==0;
}

inline Formula::PostIterator::~PostIterator()
{
  delete &myStack;
}

/**Header definitions for global functions related to formulas*/

/**@return the number of Atomic propositions*/
unsigned numAP(const class Formula &f);

/**@return the number subformulas*/
unsigned count(const class Formula &f);

/**Compute a hash value for a formula*/
unsigned hashformula(const class Formula &f);

#endif //FORMULA_H_

