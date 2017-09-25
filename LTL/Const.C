// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Const.h
 * Class for representing propositional constants true, false
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__

#include "Const.h"
#include <cstdio>

Const::Const(bool value): Formula(), myValue(value) 
{
  ;
}

Const::Const(const class Const &other) : myValue(other.myValue)
{
  ;
}

class Const &
Const::operator=(const class Const &rhs) 
{
  myValue=rhs.myValue;
  return *this;
}

void
Const::print(FILE *file) const
{
  if(myValue)
    fputs("t ", file);
  else
    fputs("f ", file);
}

bool 
Const::operator==(const class Formula &other) const
{
  if(other.getType() != fConst) 
    return false;
  return (myValue == static_cast<const class Const &>(other).myValue);
}
