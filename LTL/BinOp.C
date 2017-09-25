// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BinOp.C
 * Class for representing Binary propositional operators 
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include "BinOp.h"
#include <cstdio>

BinOp::BinOp(BinOp::Op op, const class Formula *left, 
	     const class Formula *right) : 
  Formula(), myOp(op), myLeft(left), myRight(right) 
{
  ;
} 

BinOp::BinOp(const class BinOp &other) : 
  Formula(), myOp(other.myOp)
{
  myLeft = other.myLeft->clone();
  myRight = other.myRight->clone();
} 

class BinOp &
BinOp::operator=(const class BinOp &rhs)
{
  myOp=rhs.myOp;
  myLeft=rhs.myLeft->clone();
  myRight=rhs.myRight->clone();
  return *this;
}

void
BinOp::print(FILE *file) const
{
  switch(myOp) {
  case And:
    fputs("& ", file);
    break;
  case Or:
    fputs("| ", file);
    break;
  case Impl:
    fputs("i ", file);
    break;
  case Equiv:
    fputs("e ", file);
    break;
  }
  myLeft->print(file);
  myRight->print(file);
}

bool
BinOp::operator==(const class Formula &other) const
{
  if(other.getType() != fBinOp)
    return false;
  const class BinOp &temp=static_cast<const class BinOp &>(other);
  if(myOp != temp.getOp())
    return false;
  return (*myLeft==*temp.myLeft && *myRight==*temp.myRight);
}

bool 
BinOp::eval(const NumberMap & apmap, unsigned ap) const
{
  switch(myOp) {
  case And:
    return (myLeft->eval(apmap, ap) && myRight->eval(apmap, ap)); 
  case Or:
    return (myLeft->eval(apmap, ap) || myRight->eval(apmap, ap)); 
  case Impl:
    return (!myLeft->eval(apmap, ap) || myRight->eval(apmap, ap)); 
  case Equiv:
    return ((!myLeft->eval(apmap, ap) || myRight->eval(apmap, ap)) 
	    && (!myRight->eval(apmap, ap) || myLeft->eval(apmap, ap)));     
  }
  return false;
}
