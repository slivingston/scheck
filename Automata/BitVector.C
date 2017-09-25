// ©2003 Marko Mäkelä (marko.makela@hut.fi and 
// Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BitVector.C
 * Bit vector
 */
#ifdef __GNUC__
#pragma implementation
#endif // __GNUC__
#include "BitVector.h"
/** Set the size of the vector 
 * @param size Number of bits
 */
void 
BitVector::setSize (unsigned size) 
{
    const unsigned numWords = getNumWords (size);
    if (myAllocated < numWords) {
      while (myAllocated < numWords)
	myAllocated <<= 1;
      word_t* bits = new word_t[myAllocated];
      const unsigned init = getNumWords (mySize);
      memcpy (bits, myBits, init * sizeof *myBits);
      memset (bits + init, 0, (myAllocated - init) * sizeof *myBits);
      delete[] myBits;
      myBits = bits;
    }
    mySize = size;
  }

unsigned 
BitVector::getNumWords (unsigned bits) const 
{
  const unsigned bitsPerWord = CHAR_BIT * sizeof (word_t);
  return (bits + bitsPerWord - 1) / bitsPerWord;
}
