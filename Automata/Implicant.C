#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include "Implicant.h"

/**Joint *this implicant with another and put the resulting implicant in *this.
 * If "other" is not joinable with *this, the result is undefined.
 * @param other implicant to join with *this
 */
void 
Implicant::join(const class Implicant &other) 
{
  assert(mySize==other.mySize);
  for(unsigned i=mySize; i--;) {
    //the single value where they differ
    if(myValues[i]!=other.myValues[i]) {
      myValues[i]=DC;
      return;
    }
  }
}

/**Check if we can join *this implicant and another implicant. The implicants
 * can be joined iff they differ for exactly one value and neither implcant
 * has DC as a value for that index
 * @param other 
 * @return true iff the implicants can be joined
 */
bool 
Implicant::joinable(const class Implicant &other) const 
{
  assert(mySize==other.mySize);
  unsigned count=0;
  for(unsigned i=mySize; i--; ) {
    if((*this)[i]!=other[i]) {
      count++;
      if(count > 1 || (*this)[i]==DC || other[i]==DC) {
	return false;
      }
    }
  }
  return count==1;
}
/**Check if *this implicant covers the given term
 * @param term 
 * @return true iff *this covers term
 */   
bool
Implicant::covers(unsigned term) const 
{
  for(unsigned i=0; i<mySize; i++) {
    if(myValues[i]!=DC && myValues[i]!=static_cast<enum Value>(term & 1u)) {
      return false;
    }
    term>>=1; //shift bits right
  }
  return true;
}
/*Check if this implicant covers the given implicant. An implicant other covers 
 * another implicant *this iff for all i where *this[i]!=other[i] 
 * other[i]=DC.
 * @param other  
 * @return true iff other covers *this
 */
bool
Implicant::covers(const class Implicant &other) const 
{
  assert(mySize==other.mySize);
  for(unsigned i=mySize; i--;) {
    if((*this)[i]!=other[i] && other[i]!=DC) {
      return false;
    }
  }
  return true;
}
bool 
Implicant::joinable(unsigned left, unsigned right, unsigned size)
{
  unsigned count=0;
  for(unsigned i=size; i--; ) {
    if ( (1u & left) ^ (1u & right)) {  //the bits differ
      count++;
      if(count>1) return false;
    }
    left>>=1;
    right>>=1;
  }
  return count==1;
}

bool 
operator<(const class Implicant & i1, const class Implicant &i2) 
{
  if(i1.size() != i2.size() )
    return false;
  for (unsigned i=i1.size(); i--; ) {
    if (i1[i]<i2[i]) {
      return true;
    }
    else if (i2[i]<i1[i]) {
      return false;
    }
  }
  return false;
}
