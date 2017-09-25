// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Pathologic.h
 * Container class for methods related to analysing the
 * pathologicness of an LTL formula
 */
  
#ifndef PATHOLOGIC_H_
#define PATHOLOGIC_H_

//Forward decs
class Formula;
class DetAut;
class NonDetAut;

class Pathologic {
  
 public:
  /**Constructor of the class
   *@param formula The formula given to scheck
   *@param detaut Deterministic finite automaton representing the formula
   *@param translator Commandline to execute external translator
   */
  Pathologic(const class Formula &formula, const class DetAut &detaut, const char *translator);  
  
  /**The destructor*/
  ~Pathologic();

  /**Determine if the given formula is pathologic
   *@return true if the formula is NOT pathologic, otherwise false
   */
  bool pathologic () const;
  
  /**Class for representing product state*/
  class State {
  public:   
    /**The constructor 
     *@param b State number for the Buchi automaton
     *@param c State number for the buchi complemented dfa
     */
    State(unsigned b, unsigned c) : buchi(b), complement(c) {}
    /**The copy constructor
     *@param orig The original to be copied
     */
    State(const class State &orig) : buchi(orig.buchi), 
      complement(orig.complement) {}     
    /**Equality comparison
     *@param other State to be compared with
     *@return true if the components are equal (modulo stack bits)
     */
    bool operator==(const class State &other) const {
      return getBuchi()==other.getBuchi() && getComplement()==other.getComplement();    
    }    
    /**The assignment operator. Copy components AND stack bits
     *@param rhs The right-hand side operand 
     *@return A reference to this object
     */
    State & operator=(const class State & rhs) {
      if(&rhs != this) {
	buchi=rhs.buchi;
	complement=rhs.complement;
      }
      return *this;
    }    
 
    /**@return the hash value of the state*/
    unsigned operator() () const {
      //mask the MSB of buchi and complement
      return hasher (getBuchi()) ^ hasher (getComplement());
    }
    /**@return the buchi component*/ 
    unsigned getBuchi() const {
      //return (buchi & ((~0)>>1));      
      return buchi;
    }
    /**@return the complement component*/
    unsigned getComplement() const {
      //return (complement & ((~0)>>1));    
      return complement;
    }
    /**Set the first bit*/
    void flag1() {
      buchi |= ~((~0)>>1);
    }
    /**Set the second bit*/
    void flag2() {
      complement |= ~((~0)>>1);
    }
    /**Unset the first bit*/
    void unset1() {
      buchi &= ((~0)>>1);
    }
    /**Unset the second bit*/
    void unset2() {
      complement &= ((~0)>>1);
    }
    /**@return true iff the first bit is set*/
    bool isflagged1() const {
      return (buchi & ~((~0)>>1)) > 0;
    }
    /**@return true iff the second bit is set*/
    bool isflagged2() const {
      return (complement & ~((~0)>>1)) > 0;
    }
  private:
    /**The buchi component*/
    unsigned buchi;
    /**The complement component*/
    unsigned complement;    
    /** Integer hash function by Robert Jenkins (bob_jenkins@compuserve.com)
     * <URL:http://ourworld.compuserve.com/homepages/bob_jenkins/blockcip.htm>
     * @param key       key to be hashed
     * @return          hash value of key
     */
    unsigned hasher (unsigned key) const {
      key += key << 12;
      key ^= key >> 22;
      key += key << 4;
      key ^= key >> 9;
      key += key << 10;
      key ^= key >> 2;
      key += key << 7;
      key ^= key >> 12;
      return key;
    }
  };
 
 public:
  struct statehash
  {
    /** Calculate the hash value of a state
     * @param state     state to be hashed
     * @return          the hash value
     */
    unsigned operator() (const class State& state) const {
      return state ();
    }
  };
  /**Class for the state of the non-recursive dfs*/
  class Depth {
  public:
    /**The constructor*/
    Depth(class State &s, unsigned depth, unsigned succs) :
      state(s), dfs(depth), numSucc(succs) {}
    /**Copy constructor*/
    Depth(const class Depth &depth) : state(depth.state),
      dfs(depth.dfs), numSucc(depth.numSucc) {}
    /**Equality comparison*/
    bool operator==(const class Depth &rhs) const {
      return (state==rhs.state && dfs==rhs.dfs && numSucc==rhs.numSucc);
    }
    /**Assignment operator*/
    class Depth & operator=(const class Depth &rhs) {
      if(&rhs!=this) {
	state=rhs.state;
	dfs=rhs.dfs;
	numSucc=rhs.numSucc;
      }
      return *this;
    } 
    /**State of the search*/
    class State state;
    /**Current dfs number of the state*/
    unsigned dfs;
    /**Number of unexplored successors*/
    unsigned numSucc;
  };

 private:
  /**Formula under consideration*/
  const class Formula &myFormula;
  /**Simple Buhci automaton representing the complemented automaton*/
  const class DetAut &myDetAut;
  /**Command line for executing external translator*/
  const char *myTranslator;
};

#endif //PATHOLOGIC_H_
