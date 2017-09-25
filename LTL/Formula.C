// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Formula.C
 * Abstract Base class for representing LTL formulae 
 */

#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include <stack>
#include "Formula.h"
#include "TemporalBinOp.h"
#include "TemporalUnOp.h"
#include "BinOp.h"
#include "Not.h"
#include "Atom.h"
#include "Const.h"

void 
Formula::Iterator::operator++() 
{
  if(myStack.empty()) 
    return;
  else {
    const class Formula * formula=myStack.top();
    myStack.pop();    
    enum Formula::Type type = formula->getType();
    switch(type) {
    case Formula::fTemporalBinOp:
      myStack.push(static_cast<const TemporalBinOp*>(formula)->getRHS());
      myStack.push(static_cast<const TemporalBinOp*>(formula)->getLHS());
      break;
    case Formula::fBinOp:
      myStack.push(static_cast<const BinOp*>(formula)->getRHS());
      myStack.push(static_cast<const BinOp*>(formula)->getLHS());
      break;
    case Formula::fTemporalUnOp:
      myStack.push(static_cast<const TemporalUnOp*>(formula)->getOperand());
      break;
    case Formula::fNot:
      myStack.push(static_cast<const Not*>(formula)->getOperand());
      break;
    default:
      break;
    }
  }
}


//-------------------------------------

Formula::PostIterator::PostIterator(const class Formula *formula): myStack(*new class FormulaStack()) 
{
  bool loop=true;
  myCurrent=const_cast<Formula *>(formula);
  while(loop) {
    enum Formula::Type type = myCurrent->getType();
    switch(type) {
    case Formula::fTemporalBinOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<TemporalBinOp*>(myCurrent)->getLHS();
      break;
    case Formula::fBinOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<BinOp*>(myCurrent)->getLHS();
      break;
    case Formula::fTemporalUnOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<TemporalUnOp*>(myCurrent)->getOperand();
      break;
    case Formula::fNot:
      myStack.push(myCurrent);
      myCurrent=static_cast<Not*>(myCurrent)->getOperand();
      break;
    case Formula::fAtom:
    case Formula::fConst:
      loop = false;
      break;
    }
  }
}

void 
Formula::PostIterator::operator++() 
{
  if(myStack.empty()) { 
    myCurrent=0;
    return;
  }
  class Formula * child=myCurrent;
  myCurrent=const_cast<Formula *>(myStack.top());
  enum Formula::Type type = myCurrent->getType();
  switch(type) {
  case Formula::fTemporalBinOp:
    if (child == static_cast<TemporalBinOp*>(myCurrent)->getRHS())
	break;      
    myCurrent=static_cast<TemporalBinOp*>(myCurrent)->getRHS();
    goto slide;
  case Formula::fBinOp:
    if (child == static_cast<BinOp*>(myCurrent)->getRHS())
      break;
    myCurrent=static_cast<BinOp*>(myCurrent)->getRHS();    
    goto slide;
  default: 
    break;
  } 
  myStack.pop();
  return;
  
 slide:
  bool loop=true;
  while(loop) {
    enum Formula::Type kind = myCurrent->getType();
    switch(kind) {
    case Formula::fTemporalBinOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<TemporalBinOp*>(myCurrent)->getLHS();
      break;
    case Formula::fBinOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<BinOp*>(myCurrent)->getLHS();
      break;
    case Formula::fTemporalUnOp:
      myStack.push(myCurrent);
      myCurrent=static_cast<TemporalUnOp*>(myCurrent)->getOperand();
      break;
    case Formula::fNot:
      myStack.push(myCurrent);
      myCurrent=static_cast<Not*>(myCurrent)->getOperand();
      break;
    default:
      loop = false;
      break;
    }
  }
}

//Global functions for Formulas. Headers defined in Formula.h 

#include "FormulaSet.h"
/**@return The number of subformulas*/
unsigned count(const class Formula &f)
{
  unsigned num=0;
  FormulaSet fset;
  for(Formula::Iterator i=f.newIterator(); !i.atEnd(); ++i) {
    if(fset.find(&(*i))==fset.end()) { //not seen before
      num++;
      fset.insert(&(*i));
    }    
  }
  return num;

}

/**@return the number of atomic propositions*/
unsigned numAP(const class Formula &f)
{
  unsigned numAP=0;
  FormulaSet fset;
  for(Formula::Iterator i=f.newIterator(); !i.atEnd(); ++i) {
    if((*i).getType()==Formula::fAtom && fset.find(&(*i))==fset.end()) {
      numAP++;
      fset.insert(&(*i));
    }    
  }
  return numAP;
}

/**Compute the hashvalue for a formula*/
unsigned hashformula(const class Formula &f) 
{
  unsigned value=0x9e3779b9; //golden ratio, an arbitrary value
  for(Formula::Iterator i=f.newIterator(); !i.atEnd(); ++i) {
    switch((*i).getType()) {
    case Formula::fTemporalBinOp:
      value^= value << static_cast<const class TemporalBinOp &>(*i).getOp() + 20;
      break;
    case Formula::fBinOp:
      value += value >> static_cast<const class BinOp &>(*i).getOp() + 7;
      break;
    case Formula::fTemporalUnOp:
      value^= value >> static_cast<const class TemporalBinOp &>(*i).getOp() + 12;
      break;
    case Formula::fNot:
      value += value << 15;
      break;
    case Formula::fAtom:
      value += value << static_cast<const class Atom &>(*i).getId() + 2;
      break;
    case Formula::fConst:
      value ^= value >> static_cast<const class Const &>(*i).getVal() + 24;
      break;
    }
  }
  return value;
}
