// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Automaton.h
 * Abstract base class for finite automata
 */

#ifndef AUTOMATON_H_
#define AUTOMATON_H_
#ifdef __GNUC__
#pragma interface
#endif // __GNUC__

//forward declaration of file
struct _IO_FILE;
typedef struct _IO_FILE FILE;
extern FILE *stdout;

class Automaton {
  
 public:
  /**Constructor of the class 
   * @param size The number of states in the automaton
   * @param aSize The size of the alphabet
   * @param sets The number of accepting sets
   */
  Automaton(unsigned size, unsigned aSize, unsigned sets=1);
  /**The destructor*/
  virtual ~Automaton(); 
  /**@return The number of states in the automaton*/
  unsigned size() const {return mySize;}
  /**@return The size of the alphabet*/
  unsigned alphabetSize() const {return myAlphabetSize;}  
   /**Get a successor state
   *@param source The source state
   *@param label The Label of the transition
   *@param num Index of the transition (for non-det automatons)
   *@return The destination state
   */
  virtual unsigned dest(unsigned source, unsigned label, unsigned index) const = 0;
  /**Check the number of arcs for the state and label
   * @param state
   * @param label
   * @return the number of outgoing arcs from state with label
   */
  virtual unsigned numArcs(unsigned state, unsigned label) const=0;

  /**Add a transition 
   *@param source The source state
   *@param label The label of the transition
   *@param dest The destination state
   */  
  virtual void addTransition(unsigned source, unsigned label, unsigned dest) =0;
  /**Delete a transition
   *@param source The source state
   *@param label The label of the transition
   *@param dest The destination state
   */
  virtual void deleteTransition(unsigned source, unsigned label, unsigned dest) =0;
  /**@return the number of acceptance sets*/
  unsigned getNumSets() const {return myNumSets;}
  /**Check if the state is a final state and return to which set it belongs
   *@param state 
   *@param set (output) the set the state belongs to
   *@return true iff the state is a final state
   */
  virtual bool isFinal(unsigned state, unsigned &set) const = 0;  
  /**Check if a state is an initial state
   *@param state
   *@return true iff state is an initial state
   */
  virtual bool isInitial(unsigned state) const = 0;

 protected:
  /**Number of states of the automaton*/
  unsigned mySize;
  /**Size of the alphabet*/
  unsigned myAlphabetSize;
  /**The number of accepting sets*/
  unsigned myNumSets;
};

void printAut(const class Automaton &automaton, FILE *stream=stdout);
#endif //AUTOMATON_H_

