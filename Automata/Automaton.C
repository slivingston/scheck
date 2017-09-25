// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Automaton.C
 * Abstract base class for finite automata
 */
#ifdef __GNUC__
#pragma implementation
#endif // __GNUC__
#include "Automaton.h"
#include <cstdio>


Automaton::Automaton(unsigned size, unsigned aSize, unsigned sets) 
  : mySize(size), myAlphabetSize(aSize), myNumSets(sets) {;} 


Automaton::~Automaton() {;}

/**Print an automaton without labels to a stream
 *@param automaton Automaton to be printed
 *@param stream
 */
void printAut(const class Automaton &automaton, FILE *stream) 
{
  fprintf(stream, "%u %u\n", automaton.size(), automaton.getNumSets());  
  for(unsigned i=automaton.size(); i--;) {
    fprintf(stream, "%u", i); (automaton.isInitial(i)) ? fputs(" 1 ", stream) : fputs(" 0 ", stream); 
    unsigned set=0;
    (automaton.isFinal(i, set)) ? fprintf(stream, "%u -1\n", set) : fputs("-1 \n", stream); 
    for(unsigned j=automaton.alphabetSize(); j--; ) {
      for(unsigned k=automaton.numArcs(i,j); k--; ) {
	fprintf(stream, "%u p%u\n", automaton.dest(i,j,k), j);
      }
    }
    fputs("-1\n", stream);
  }  
  return;
}
