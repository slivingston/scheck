// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file TemporalBinOp.C
 * Class for representing the Temporal Binary operators 
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include "TemporalBinOp.h"
#include <cstdio>

TemporalBinOp::TemporalBinOp(TemporalBinOp::Op op, 
			     const class Formula *left, 
			     const class Formula *right) :
  Formula(), myOp(op), myLeft(left), myRight(right)
{
  ;
}

TemporalBinOp::TemporalBinOp(const class TemporalBinOp &other) :
  Formula(), myOp(other.myOp)
{
  myLeft=other.myLeft->clone();
  myRight=other.myRight->clone();
}

class TemporalBinOp &
TemporalBinOp::operator=(const class TemporalBinOp &rhs)
{
  myOp=rhs.myOp;
  myLeft=rhs.myLeft->clone();
  myRight=rhs.myRight->clone();
  return *this;
}

void
TemporalBinOp::print(FILE *file) const
{
  switch(myOp) {
  case Until:
    fputs("U ", file);
    break;
  case Release:     
    fputs("V ", file);
    break;
  }
  myLeft->print(file);
  myRight->print(file);
}

bool
TemporalBinOp::operator==(const class Formula &other) const
{
  if(other.getType() != fTemporalBinOp)
    return false;
  const TemporalBinOp &temp = static_cast<const TemporalBinOp &>(other);
  if(temp.getOp() != myOp)
    return false;
  return (*myLeft==*temp.myLeft && *myRight==*temp.myRight);  
}
