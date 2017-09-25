// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file TemporalUnOp.C
 * Class for representing the unary Temporal operators 
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include "TemporalUnOp.h"
#include <cstdio>

TemporalUnOp::TemporalUnOp(TemporalUnOp::Op op, 
			   const class Formula *formula) :
  Formula(), myOp(op), myFormula(formula)
{
  ;
}

TemporalUnOp::TemporalUnOp(const class TemporalUnOp &formula) :
  Formula(), myOp(formula.myOp)
{
  myFormula=formula.myFormula->clone();
}

class TemporalUnOp &
TemporalUnOp::operator=(const class TemporalUnOp &rhs)
{
  myOp=rhs.myOp;
  myFormula=rhs.myFormula->clone();
  return *this;
}

void
TemporalUnOp::print(FILE *file) const 
{
  switch(myOp) {
  case Finally:
    fputs("F ", file);
    break;
  case Globally:
    fputs("G ", file);
    break;
  case Next:
    fputs("X ", file);
    break;
  }
  myFormula->print(file);
}

bool
TemporalUnOp::operator==(const class Formula &other) const
{
  if (other.getType() != fTemporalUnOp)
    return false;
  const class TemporalUnOp &temp=static_cast<const class TemporalUnOp &>(other);
  if(myOp != temp.getOp())
    return false;
  return (*myFormula==*temp.myFormula);
}
