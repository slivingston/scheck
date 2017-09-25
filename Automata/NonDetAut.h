// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file NonDetAut.h
 * Class for Non-deterministic automaton
 */

#ifndef NONDETAUT_H_
#define NONDETAUT_H_
#ifdef __GNUC__
#pragma interface
#endif //__GNUC__
#include "Automaton.h"
#include "TransRel.h"
#include "BitVector.h"

class NonDetAut : public Automaton  {

 public:
  
  /**Constructor of the class
   * @param size The number of states in the automaton
   * @param aSize The size of the alphabet
   * @param sets The number of accepting sets 
   */
  NonDetAut(unsigned size, unsigned aSize, unsigned sets);
  /**The destructor*/
  ~NonDetAut();
  /**Add transition
   * @param source Id of the source state
   * @param label Label of the transition
   * @param dest Id of the destination state
   */
  void addTransition(unsigned source, unsigned label, unsigned dest) {
    assert(source<mySize && label<myAlphabetSize && dest<mySize);
    myTransRel.insert(TransRel::value_type(UIPair(source, label), dest));
    return;
  }
  /**Delete a transition
   * @param source Id of the source state
   * @param label Label of the transition
   * @param dest Id of the destination state
   */
  void deleteTransition(unsigned source, unsigned label, unsigned dest) {
    assert(source <= mySize && label<myAlphabetSize && dest<mySize);
    for(std::pair<TransRel::iterator, TransRel::iterator> p=myTransRel.equal_range(UIPair(source, label));
	p.first!=p.second; ++p.first) {
      if((*p.first).second == dest) {
	myTransRel.erase(p.first);
	return;
      }
    }
  }
  /**Return the name of a target state. The method does not fail even if the
   * requested destination does not exist. Use the method numArcs to check
   * the given result is correct.
   *@param source state
   *@param label The arc label
   *@index Which arc
   *@return The name of target state
   */
  unsigned dest(unsigned source, unsigned label, unsigned index) const {
    assert(source < mySize && label<myAlphabetSize);
    unsigned j=0;
    for(std::pair<TransRel::const_iterator, TransRel::const_iterator> 
	  p=myTransRel.equal_range(UIPair(source, label));
    	p.first!=p.second; ++p.first) {
      if(index == j++ ) {
      	return (*p.first).second;
      }
    }
    return 0;
  }  
  /**@return Number of arcs with the given source and label*/  
  unsigned numArcs(unsigned source, unsigned label) const {
    assert(source<mySize && label<myAlphabetSize);
    return myTransRel.count(UIPair(source, label));
  }
  /*Check if a state belongs to a final set
   * @param state
   * @param set (output) the set the state belongs to
   * @return true iff the state belongs to a final set
   */
  bool isFinal(unsigned state, unsigned &set) const {
    assert(state<mySize);
    if(myFinalSets[state]) {
      set=myFinalSets[state]-1;
      return true;
    }
    return false;
  }
   /*Check if a state belongs to a final set
   * @param state
   * @param set (output) the set the state belongs to
   * @return true iff the state belongs to a final set
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
  }
  /**@return true iff state is an initial state*/
  bool isInitial(unsigned state) const {
    assert(state<mySize);
    return myInitial[state];
  }     
  /**Make a state an inital state
   * @param state
   */
  void setInitial(unsigned state) {
    assert(state<mySize);
    myInitial.assign(state, true);
  } 
  /**@return an initial state*/
  unsigned getInitial() const {
    for(unsigned i=0; i<mySize; i++)
      if(isInitial(i)) 
	return i;
    return 0;
  }
  /**Increase the size of the automaton*/
  void grow(unsigned size, unsigned asize);
  /**Create a finite automaton corresponding to a safety formula     
   *@param f formula to be translated
   *@return the corresponding finite automaton
   *@precond f must be in negation normal form
   */  
  static NonDetAut* create(const class Formula &f); 
  /**Determinze this nondet automaton
   *@param result Place holder for the result
   */
  void determinize(class DetAut &result) const;
 
 private:
  /**The transition relation*/
  TransRel myTransRel;
  /**Store acceptance sets for state*/
  unsigned *myFinalSets;
  /**Initial state information*/
  class BitVector myInitial;
};

/**Create a Büchi automaton using an exteranal translator
 * @param translator commmandline of the external tool to use
 * @param f Formula to be translated
 * @return Büchi automaton with the same language as the formula
 */
NonDetAut *buchiread(const char *translator, const class Formula &f);

#endif //NONDETAUT_H_

