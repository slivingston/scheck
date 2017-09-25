#ifndef IMPLICANT_H_
#define IMPLICANT_H_
#ifdef __GNUC__
#pragma interface 
#endif //__GNUC__

#include <cassert>

/**Class for storing an implicant, i.e. value distribution for a conjuction  of varibles.
 * Possible values are False, True, and Don't Care (DC)
 */
class Implicant {
 public:
  enum Value {False, True, DC};
  /**Constructor of the class
   * @param size The number of variables
   */
  explicit Implicant(unsigned size): mySize(size) {
    assert(size);
    myValues = new Value[mySize];
    for(unsigned i=mySize; i--; ) {
      myValues[i]=False;
    }
  }
  /**Initialise the implicant with an array
   *@param implicant array to use for initialisation
   *@unsigned size Size of the array
   */
  explicit Implicant(const unsigned implicant[], unsigned size) : mySize(size) {
    assert(size>0);
    myValues = new enum Value[mySize];
    //values > 2 are interpreted as DC
    for(unsigned i=mySize; i--; ) {
      if (implicant[i]>2) {
	myValues[i]=DC;
      }
      else {
	myValues[i]=static_cast<enum Value>(implicant[i]);
      }
    }
  }
  /**Initialise the implicant with a term
   * @param term 
   * @param size Number of elements in the term
   */
  explicit Implicant(unsigned term, unsigned size) : mySize(size) {
    myValues=new enum Value[mySize];
    for(unsigned i=0; i<mySize; i++) {
      if(term & 1u) {
	myValues[i]=True;
      }
      else {
	myValues[i]=False;
      }
      term>>=1;
    }
  }

  /**The copy constructor 
   * @param other Implicant to initialise this one with
   */
  explicit Implicant(const class Implicant &other) : mySize(other.mySize) {
    myValues = new enum Value[mySize];
    for(unsigned i=mySize; i--; ) {
      myValues[i]=other[i];
    }
  }
  /**The destructor*/
  ~Implicant() {
    delete [] myValues;
  }
  /**The assignment operator
   *@param rhs 
   *@return *this
   */
  class Implicant & operator=(const class Implicant &rhs) {
    assert(mySize==rhs.mySize);
    //check for assignment to self
    if(&rhs==this) return *this;
    for(unsigned i=mySize; i--; ) {
      (*this)[i]=rhs[i];
    }
    return *this;
  }

  /**@return the value of the varible with index varindex*/
  enum Value operator[](unsigned varindex) const {
    assert(varindex<mySize);
    return myValues[varindex];
  }
  enum Value & operator[](unsigned varindex) {
    assert(varindex<mySize);
    return myValues[varindex];
  }
  /**@return number of atoms in the implicant*/
  unsigned size() const {
    return mySize;
  }
  /**Equality comparison 
   * @param rhs Implicant to compare with
   * @return true iff for all i *this[i]==rhs[i]
   */
  bool operator==(const class Implicant &rhs) const {
    if(mySize!=rhs.mySize) return false;
    for(unsigned i=mySize; i--; ) {
      if((*this)[i]!=rhs[i]) {
	return false;
      }
    }
    return true;
  }
  /*Check if this implicant covers the given implicant. An implicant other covers 
   * another implicant *this iff for all i where *this[i]!=other[i] 
   * other[i]=DC.
   * @param other  
   * @return true iff other covers *this
   */
  bool covers(const class Implicant &other) const;
  /**Check if *this implicant covers the given term
   * @param term 
   * @return true iff *this covers term
   */   
  bool covers(unsigned term) const;
  /**Check if the left term is joinable with the rigth term
   * @param left The left term
   * @param right The right term
   * @param size Number of atoms in the terms 
   * @return true iff left can be joined with right
   */   
  static bool joinable(unsigned left, unsigned right, unsigned size) ;
  /**Check if we can join *this implicant and another implicant. The implicants
   * can be joined iff they differ for exactly one value and neither implcant
   * has DC as a value for that index
   * @param other 
   * @return true iff the implicants can be joined
   */
  bool joinable(const class Implicant &other) const;   
  /**Join *this implicant with another and put the resulting implicant in *this.
   * If "other" is not joinable with *this, the result is undefined.
   * @param other implicant to join with *this
   */
  void join(const class Implicant &other);

private:
  /**Array for storing values*/
  enum Value *myValues;
  /**Size of the array*/
  unsigned mySize;
};

bool operator<(const class Implicant &i1, const class Implicant &i2);

#endif //IMPLICANT_H_
