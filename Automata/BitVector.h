// ©2003 Marko Mäkelä (marko.makela@hut.fi and 
// Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file BitVector.h
 * Binary digit (bit) vector 
 */
#ifndef BITVECTOR_H_
# define BITVECTOR_H_
#ifdef __GNUC__
# pragma interface
#endif // __GNUC__
# include <climits>
# include <cstring>
# include <cassert>

/** @file BitVector.h
 * Bit vector
 */

/** Binary digit (bit) vector */
class BitVector
{
public:
  /** machine word */
  typedef unsigned word_t;

  /** Constructor
   * @param size	number of elements in the vector
   */
  explicit BitVector (unsigned size = 0) :
    mySize (0), myAllocated (1), myBits (new word_t[1]) {
    *myBits = 0;
    setSize (size);
  }
  /** Copy constructor */
  explicit BitVector (const class BitVector& old) :
    mySize (old.mySize), myAllocated (old.getNumWords (old.mySize)),
    myBits (0) {
    memcpy (myBits = new word_t[old.myAllocated], old.myBits,
	    myAllocated * sizeof (word_t));
  }

  /** Assignment operator */
  class BitVector& operator= (const class BitVector& old) {
    assert(mySize == old.mySize && myAllocated == old.myAllocated);
    memcpy(myBits, old.myBits, myAllocated * sizeof (word_t));
    return *this;
  }

public:
  /** Destructor */
  ~BitVector () { delete[] myBits; }

  /** Determine the number of words the specified amount of bits occupy */
  unsigned getNumWords (unsigned bits) const;
  //{
  //  const unsigned bitsPerWord = CHAR_BIT * sizeof (word_t);
  //  return (bits + bitsPerWord - 1) / bitsPerWord;
  //}

  /** Clear the vector */
  void clear () {
    memset (myBits, 0, myAllocated * sizeof *myBits);
  }

  /** Truncate the vector
   * @param size	number of elements to leave in the vector
   */
  void truncate (unsigned size = 0) {
    if (mySize <= size) return;
    mySize = size;
    const unsigned init = getNumWords (size);
    if (const unsigned rest = size % (CHAR_BIT * sizeof (word_t)))
      assert (init >= 1), myBits[init - 1] &= (1u << rest) - 1;
    memset (myBits, 0, (myAllocated - init) * sizeof *myBits);
  }
  /** Set the size of the vector 
   * @param size Number of bits
   */
  void setSize (unsigned size); 
  /** Determine the size of the vector 
   * @return Number of bit positions in the vector
   */
  unsigned getSize () const { return mySize; }

  /** Read a binary digit
   * @param index	zero-based index of the element
   * @return		value of the ternary digit
   */
  bool operator[] (unsigned index) const {
    assert (index < mySize);
    return myBits[index / (CHAR_BIT * sizeof (word_t))] &
      (1u << (index % (CHAR_BIT * sizeof (word_t))));
  }

  /**Equality comparison*/
  bool operator==(const class BitVector &other) const{
    assert(mySize == other.mySize);
    for(unsigned i=getNumWords(mySize); i--; )
      if(myBits[i]!=other.myBits[i])
	return false;
    return true;
  }

  /** Assign a binary digit
   * @param index	zero-based index of the element
   * @param value	new value of the digit
   */
  void assign (unsigned index, bool value) {
    assert (index < mySize);
    word_t& word = myBits[index / (CHAR_BIT * sizeof (word_t))];
    word_t bit = 1u << (index % (CHAR_BIT * sizeof (word_t)));
    if (value)
      word |= bit;
    else
      word &= ~bit;
  }

  /** Test and set: read and set a binary digit
   * @param index	zero-based index of the element
   * @return		whether the element already was set
   */
  bool tset (unsigned index) {
    assert (index < mySize);
    word_t& word = myBits[index / (CHAR_BIT * sizeof (word_t))];
    word_t bit = 1u << (index % (CHAR_BIT * sizeof (word_t)));
    if (word & bit)
      return true;
    word |= bit;
    return false;
  }

  /** Test and reset: read and reset a binary digit
   * @param index	zero-based index of the element
   * @return		whether the element already was set
   */
  bool treset (unsigned index) {
    assert (index < mySize);
    word_t& word = myBits[index / (CHAR_BIT * sizeof (word_t))];
    word_t bit = 1u << (index % (CHAR_BIT * sizeof (word_t)));
    if (!(word & bit))
      return false;
    word &= ~bit;
    return true;
  }

  /** Assign a binary digit, extending the vector with zeros if required
   * @param index	zero-based index of the element
   * @param value	new value of the digit
   */
  void extend (unsigned index, bool value) {
    if (index >= mySize) setSize (index + 1);
    assign (index, value);
  }

  /** Determine whether the whole vector is filled with zero bits */
  bool allClear () const {
    unsigned i = mySize / (CHAR_BIT * sizeof (word_t));
    if (mySize && (myBits[i] &
		   ((1u << (mySize % (CHAR_BIT * sizeof (word_t)))) - 1)))
      return false;
    while (i--) if (myBits[i]) return false;
    return true;
  }

  /** Determine whether the whole vector is filled with one bits */
  bool allSet () const {
    unsigned i = mySize / (CHAR_BIT * sizeof (word_t));
    if (mySize && ~(myBits[i] |
		    ~((1u << (mySize % (CHAR_BIT * sizeof (word_t)))) - 1)))
      return false;
    while (i--) if (~myBits[i]) return false;
    return true;
  }

  /** Compute bit wise disjunction with the complement of a bit vector
   * @param other	bit vector whose complement is to be ORed with this
   */
  void orNot (const class BitVector& other) {
    assert (mySize == other.mySize);
    for (unsigned i = getNumWords (mySize); i--; )
      myBits[i] |= ~other.myBits[i];
  }

  /** Compute bit wise conjunction with the complement of a bit vector
   * @param other	bit vector whose complement is to be ANDed with this
   * @return		true if any bits were left '1' after the operation
   */
  bool andNot (const class BitVector& other) {
    assert (mySize == other.mySize);
    bool clear = true;
    for (unsigned i = getNumWords (mySize); i--; )
      if (myBits[i] &= ~other.myBits[i])
	clear = false;
    return !clear;
  }
  

  /** Check if the other bit vector is disjoint w.r.t this one
   * @param other bit vector to be checked against
   * @return true if the bitvectors are disjoint
   */
  bool andPos(const class BitVector &other) const {
    assert (mySize == other.mySize);
    bool clear = true;
    for (unsigned i = getNumWords (mySize); i--; ) {
      if (myBits[i] & other.myBits[i]) {
	clear = false;
	break;
      }
    }
    return !clear;
  }

  /**@return hashvalue of the contents*/
  int hash(int seed=0x9e3779b9) const {
    for(unsigned i = getNumWords (mySize); i--; )
      seed ^= myBits[i];
    return seed;
  }
  
  /**@return Number of true bits in the vector*/
  unsigned numTrue() const {
    unsigned count=0;
    for(unsigned i = getNumWords (mySize); i--; )
      if((*this)[i])
	count++;
    return count;
  }

private:
  /** Number of elements in the vector */
  unsigned mySize;
  /** Size of the allocated vector in words */
  unsigned myAllocated;
  /** The vector */
  word_t* myBits;
};

#endif // BITVECTOR_H_
