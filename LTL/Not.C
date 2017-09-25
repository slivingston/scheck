// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Not.C
 * Class for representing the negation of a formula
 */
#ifdef __GNUC__
#pragma implementation
#endif // __GNUC__
#include "Not.h"
#include <cstdio>

Not::Not(const class Formula *formula) : Formula(), myFormula(formula) 
{
  ;
}

Not::Not(const class Not &other) : Formula()
{
  myFormula = other.myFormula->clone();
}

class Not &
Not::operator=(const class Not &rhs)
{
  myFormula = rhs.myFormula->clone();
  return *this;
}

void
Not::print(FILE *file) const 
{
  fputs("! ", file); 
  myFormula->print(file);
}

bool
Not::operator==(const class Formula &other) const
{
  if(other.getType() != fNot)
    return false;
  const class Not &temp=static_cast<const class Not &>(other);
  return (*myFormula==*temp.getOperand());
}

bool 
Not::eval(const NumberMap &apmap, unsigned ap) const
{
  return !myFormula->eval(apmap, ap);
}
