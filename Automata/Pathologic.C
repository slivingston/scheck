// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file Pathologic.C
 * Container class for methods related to analysing the
 * pathologicness of an LTL formula
 */
  
#include "Pathologic.h"
#include "Formula.h"
#include "NonDetAut.h"
#include "DetAut.h"
#include "BitVector.h"
#include "ExtHashMap.h"
#include <list>

typedef Sgi::hash_map<class Pathologic::State, unsigned, Pathologic::statehash> StateMap;
typedef std::list<class Pathologic::State> StateStack;
typedef std::list<class Pathologic::Depth> DepthStack;

Pathologic::Pathologic(const class Formula &formula, const class DetAut &detaut, const char *translator) 
  : myFormula(formula), myDetAut(detaut), myTranslator(translator) {}


Pathologic::~Pathologic() 
{
}

/**Check if state is in the map
 * @param visited The map to query
 * @param s The state to use in the query
 * @return 0 if s is not in visited, otherwise its depth number
 */
inline static unsigned
lookup(StateMap &visited, Pathologic::State &s)
{
  StateMap::const_iterator i=visited.find(s);
  return (i==visited.end()) ? 0 : (*i).second;

}


bool 
Pathologic::pathologic() const
{
  /**Generalised buchi automaton of the formula*/
  class NonDetAut *buchi=buchiread(myTranslator, myFormula);
  assert(buchi);
  /**Buchi automaton from complementing DFA*/
  class NonDetAut *complement=myDetAut.buchiComplement();
  assert(buchi->alphabetSize() == complement->alphabetSize());
  /**Map over visted states*/
  StateMap visited;
  /**Stack for generated children*/
  StateStack children;
  /**Stack for dfs*/
  DepthStack dfsStack;
  /**Father stack for storing components*/
  StateStack father;
  /**Flag if state has a selfloop*/
  class BitVector selfloops;
  /**Current depth first number*/
  unsigned depth=1;
  
  class State s(buchi->getInitial(), complement->getInitial());
  for(;;) {
    //depth first search
    for(;;) {
      std::pair<StateMap::iterator,bool> p =
      visited.insert (StateMap::value_type (s, depth));
      if (!p.second) break;
      father.push_front(s);	    
      unsigned enabled=0;
      for(unsigned label=buchi->alphabetSize(); label--; ) {
	for (unsigned index_b=buchi->numArcs(s.getBuchi(), label); index_b--; ) {
	  for(unsigned index_c=complement->numArcs(s.getComplement(), label); index_c--; ) {
	    children.push_front (State(buchi->dest(s.getBuchi(), label, index_b), 
				       complement->dest(s.getComplement(), label, index_c)));
	    ++enabled;				
	    if (s.getBuchi() == buchi->dest(s.getBuchi(), label, index_b) 
		&& s.getComplement() == complement->dest(s.getComplement(), label, index_c)) 
	      selfloops.extend (depth, true);
	  }
	}
	
      }  
      dfsStack.push_front(Depth(s, depth++, enabled));
      if (!enabled) break;
      s = children.front();
      children.pop_front();
    }
    
    unsigned minDepth = visited[s];
    //backtrack
    for (;;) {
      if(dfsStack.empty()) {
	delete complement;
	delete buchi;
	return true;
      }
      class Depth & top= dfsStack.front();
      unsigned sDepth = top.dfs;     
      if (minDepth && minDepth < sDepth)
        top.dfs = sDepth = minDepth; 
      if (top.numSucc-- > 1) {
	assert(!children.empty());
	s=children.front();
	children.pop_front();
	break;
      }
      s = top.state;
      dfsStack.pop_front ();
      
      if (sDepth == ::lookup (visited, s)) { //component found
        const unsigned maxDepth = visited[father.front ()];
        assert (maxDepth >= sDepth);
        selfloops.setSize (maxDepth + 1);
        class BitVector visitedSets (buchi->getNumSets ()+1);
        for (depth = maxDepth + 1; depth > sDepth; depth--) { 
          const State &state=father.front();
          visited[state]=0;
          for (unsigned set=buchi->getNumSets(); set--; ) { //gen buchi sets are from 1..numBuchiSets
            if (buchi->isFinal(state.getBuchi(), set)) visitedSets.assign (set+1, true);        
          }
          if (complement->isFinal(state.getComplement())) visitedSets.assign (0, true); //complement set is 0            
          father.pop_front ();                            
        }
	if ((maxDepth>sDepth || selfloops[maxDepth]) && visitedSets.allSet ()) {
	  delete complement;
          delete buchi;     
          return false;   
        }
	selfloops.truncate (depth = sDepth);	
      }
      minDepth = top.dfs;
    }
  }
  return true;   
} //end
