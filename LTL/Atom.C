// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Atom.C
 * Class for representing atomic propositions 
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__

#include "Atom.h"
#include <cstdio>

Atom::Atom(unsigned number, bool sign):Formula(), myId(number), mySign(sign)
{
  ;
} 

Atom::Atom(const class Atom &other) : myId(other.myId), mySign(other.mySign)
{
  ;
}

class Atom &
Atom::operator=(const class Atom &rhs)
{
  myId=rhs.myId;
  mySign=rhs.mySign;
  return *this;
}

void
Atom::print(FILE *file) const
{
  if(mySign)
    fprintf(file, "p%u ", myId);
  else
    fprintf(file, "! p%u ", myId);
}

bool 
Atom::operator==(const class Formula &other) const
{
  if(other.getType() != fAtom)
    return false;
  const class Atom &rhs=static_cast<const class Atom &>(other);
  return (myId == rhs.myId && mySign == rhs.mySign);
}
