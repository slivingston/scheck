// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file DetAut.h
 * Finite deterministic automaton
 */

#ifndef DETAUT_H_
#define DETAUT_H_
#ifdef __GNUC__
#pragma interface
#endif // __GNUC__

#include "Automaton.h"
#include "PairMap.h"


class NonDetAut; //forward declaration

class DetAut : public Automaton {
  
 public:
  /**Constructor of the class
   * @param size Number of states in the automaton
   * @param aSize Size of the alphabet
   * @param sets The number of accepting sets
   */
  DetAut(unsigned size, unsigned aSize, unsigned sets);
  /**The destructor*/
  ~DetAut();
  /**Get the successor of a state
   * @param source The source state
   * @param label The label of the transition
   * @return The destination state
   */
  unsigned dest(unsigned source, unsigned label, unsigned index=0) const {
    assert(source<mySize && label<myAlphabetSize);
    return myTransRel.get(source, label);    
  }
  /**Return the number of successors for a state with a given label
   * @param state
   * @param label
   * @return the number of successors 
   */
  unsigned numArcs(unsigned state, unsigned label) const {
    return (myTransRel.isEmpty(state, label)) ? 0 : 1; 
  }
  /**Add a transition
   * @param state
   * @param label
   * @param dest
   */
  void addTransition(unsigned state, unsigned label, unsigned dest) {
    myTransRel.set(state, label, dest);
  }
  /**Delete a transition
   * @param state
   * @param label 
   */
  void deleteTransition(unsigned state, unsigned label, unsigned index=0) {
    myTransRel.clear(state, label);
  }
  /**Check if a state belongs to a accepetance set
   * @param state  
   * @param set (output) the set the state belongs to
   * @return true iff state belongs to an acceptance set
   */
  bool isFinal(unsigned state, unsigned &set) const {
    assert(state<mySize);
    if(myFinalSets[state]) {
      set=myFinalSets[state]-1;
      return true;
    }
    return false;
  }
  /**Check if a state belongs to a final set
   * @param state
   * @return true iff belongs to an acceptance set
   */
  bool isFinal(unsigned state) const {
    assert(state<mySize);
    return (myFinalSets[state]) ? true : false;
  }

  /**Make a state a final state
   * @param state
   * @param set
   */
  void makeFinal(unsigned state, unsigned set=0) {
    assert(state<mySize && set < myNumSets);
    myFinalSets[state]=set+1;
    myNumFinal++;
  }
  /**Unset the final state flag for a state
   * @param state
   */
  void delFinal(unsigned state) {
    assert(state < mySize);
    myFinalSets[state]=0;
  }

  /**@return true iff state is the initial state*/
  bool isInitial(unsigned state) const {return (state==myInitial);}

  /**@return the initial state of the automaton*/
  unsigned initial() const {return myInitial;}
  /**Set the initial state
   *@param state the new initial state
   */
  void setInitial(unsigned state) {
    assert(state<mySize);
    myInitial=state;
  }
  /**Increase the size of the automaton
   *@param size the new size of the automaton
   *@param asize the size of the alphabet
   */
  void grow(unsigned size, unsigned asize);
  /**Construct a minimised version of the automaton using
   * Hopcroft's algorithm. Remember to deallocate the produced automaton.
   * @return a minimal automaton   
   */
  DetAut * minimise() const;
  /**Interpret the automaton as a Büchi automaton and construct its complement 
   * @return the complement of the Büchi automaton
   */ 
  NonDetAut *buchiComplement() const;

 private:
  class PairMap myTransRel;
  unsigned *myFinalSets;
  unsigned myInitial;
  unsigned myNumFinal;
};

void removeSink(class DetAut &result);


#endif //DETAUT_H_
