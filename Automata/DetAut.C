// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file DetAut.C
 * Finite deterministic automaton
 */
#ifdef __GNUC__
# pragma implementation
#endif // __GNUC__

#include "DetAut.h"
#include "NonDetAut.h"
#include <cstring>
#include "BVSet.h"
#include "BVPair.h"

DetAut::DetAut(unsigned size, unsigned aSize, unsigned sets) : Automaton(size, aSize, sets), 
							       myTransRel(size, aSize), myInitial(0), myNumFinal(0)
{
  myFinalSets=new unsigned[size]; assert(myFinalSets);
  for(unsigned i=size; i--; ) myFinalSets[i]=0;
}

DetAut::~DetAut() 
{
  delete[] myFinalSets;
}

void 
DetAut::grow(unsigned size, unsigned asize) 
{
  if(size<mySize) {
    mySize=size;
    return;
  }
  if (size > mySize) {
    myTransRel.grow(size, myAlphabetSize);
    unsigned *sets=new unsigned[size]; assert(sets);
    memcpy(sets, myFinalSets, mySize * sizeof *sets);
    memset(sets+mySize, 0, (size-mySize) * sizeof *sets);
    delete[] myFinalSets;
    myFinalSets=sets;
    mySize=size;
  }
  if(asize>myAlphabetSize)
    myAlphabetSize=asize;
}

/**@return Can the two sets be split w.r.t. to a certain label*/
inline static bool
splittable(const class DetAut &aut, const class BitVector &q0, 
           const class BitVector &q1, unsigned label) 
{
  for(unsigned p=q0.getSize(); p--; )  {
    if(q0[p]) {
      for(unsigned q=p; q--; ) {
        if(q0[q]) {       
          if( ((aut.numArcs(p, label) && q1[aut.dest(p, label)]) && 
               (!aut.numArcs(q, label) || !q1[aut.dest(q, label)])) ||
              ((!aut.numArcs(p, label) || !q1[aut.dest(p, label)]) && 
               (aut.numArcs(q, label) && q1[aut.dest(q, label)])) )
            return true;
        }
      }
    }
  }
  return false;
} 

/**Compute a relative transitive closure
 *@param aut Automaton concerned
 *@param q0 Set of start states
 *@param q1 All dests must be in this set
 *@result place holder for the result
 */
inline static void
closure(const DetAut &aut, const class BitVector &q0, 
        const class BitVector &q1, unsigned label, class BitVector &result)
{
  for (unsigned p=q0.getSize(); p--; ) {
    if(q0[p] && aut.numArcs(p, label) && q1[aut.dest(p,label)])
      result.assign(p, true);
  }
}


DetAut*
DetAut::minimise() const
{
  BVSet P;
  BitVector accepting(mySize);
  for(unsigned i=mySize; i--; ) {
    if(myFinalSets[i]) {
      accepting.assign(i, true);
    }
  }
  //init P
  P.insert(accepting);  
  BitVector allStates(mySize); 
  for(unsigned i=allStates.getSize(); i--; )
    allStates.assign(i, true);
  allStates.andNot(accepting);
  P.insert(allStates);

  //init L
  BVPair L; 
  if(myNumFinal <= (mySize - myNumFinal)) 
    for (unsigned i=myAlphabetSize; i--; )
      L.insert(BVPair::value_type(accepting,i)); 
  else {
    for (unsigned i=myAlphabetSize; i--; )
      L.insert(BVPair::value_type(allStates,i));
  }
  
  while(!L.empty()) {
    Pair pair = *L.begin();
    L.erase(L.begin());
    const BitVector &q1=pair.first;
    BVSet POld(P);
    for(BVSet::const_iterator iter=POld.begin(); iter!=POld.end(); ++iter) {
      const BitVector &q0=*iter;
      if (!splittable(*this, q0, q1, pair.second))
        continue;
      BitVector q0prime(mySize);
      closure(*this, q0, q1, pair.second, q0prime);
      P.erase(q0);
      BitVector q0Minus(q0);
      q0Minus.andNot(q0prime);
      P.insert(q0prime);
      P.insert(q0Minus);      
      for(unsigned i=0; i<myAlphabetSize; ++i) {
        BVPair::iterator j=L.find(BVPair::value_type(q0, i));
        if (j != L.end()) {
          L.erase(j);
          L.insert(Pair(q0prime, i));
          L.insert(Pair(q0Minus, i));
        }
        else {
          if (q0prime.numTrue() <= q0Minus.numTrue())
            L.insert(Pair(q0prime, i)) ;
          else
            L.insert(Pair(q0Minus, i));
        }
      }    
    }
  }
 //construct the reduced Automaton
  unsigned size=P.size();
  class DetAut &result=*new DetAut(size, myAlphabetSize, 1);
  unsigned source=0;
  for (BVSet::const_iterator i=P.begin(); i!=P.end(); ++i, source++) {    
    for(unsigned v=myAlphabetSize; v--; ) {
      unsigned dest=0;  
      for (BVSet::const_iterator j=P.begin(); j!=P.end(); ++j, dest++) {
        for(unsigned p=(*i).getSize(); p--; ) {
          if ((*i)[p]) {
            if(accepting[p])
              result.makeFinal(source);
            if(myInitial==p)
              result.setInitial(source);               
            if ((*j)[(this->dest(p, v))]) {            
              result.addTransition(source, v, dest);
              break;
            }
          }
        }
      }
    }
  }   
  return &result;
}

NonDetAut *
DetAut::buchiComplement() const 
{
  NonDetAut *result=new NonDetAut(2*mySize, myAlphabetSize,1);
  for(unsigned state=mySize; state--;) {
    if(this->isInitial(state)) result->setInitial(state);
    result->makeFinal(mySize+state);                
    for(unsigned action=myAlphabetSize; action--; ) {
      if(!(this->numArcs(state, action))) continue;
      result->addTransition(state, action, this->dest(state, action));
      if(!this->isFinal(this->dest(state, action))) {
        result->addTransition(state, action, mySize+(this->dest(state, action)));
        result->addTransition(mySize+state, action, mySize+(this->dest(state, action)));
      }
    }    
  }
  return result;

}

/**Remove a sink state from a complete deterministic automaton
 *@param result Automaton to be processed
 */
void
removeSink(class DetAut &result)
{
  
//check for a sink state 
  unsigned sink=0;
  bool flag=false;
  for(unsigned i=result.size(); i--  && !flag; ) {
    if(result.isInitial(i) || result.isFinal(i))
      continue;    
    for(unsigned j=result.alphabetSize(); j-- ; ) {
      sink=result.dest(i,j);
      if(i!=sink) 
	break;
      if(j==0) {//i is a sink
	flag=true;
      }
    }
  }

  if(!flag) return;
  //remove the sink state
  for(unsigned i=0; i<sink; i++) {
    for(unsigned j=result.alphabetSize(); j--; ) {
      if (result.numArcs(i,j)==0) continue;
	  if(result.dest(i,j) > sink)	//implicit delete     	    
	    result.addTransition(i, j, result.dest(i,j) - 1);
	  else if(result.dest(i,j) == sink)
	    result.deleteTransition(i, j); 
    }
  }    
    //move the transitions of the states with an index 
    //greater than the sink to one smaller index
  for(unsigned i=sink; i<result.size()-1; i++) {
    if(result.isFinal(i+1)) {
      result.delFinal(i+1);
      result.makeFinal(i);
    }
    if(result.isInitial(i+1)) {
      result.setInitial(i);
    }
    for(unsigned j=result.alphabetSize(); j--; ) {
      //delete the old transition
      result.deleteTransition(i, j);
      if (result.numArcs(i+1,j)==0) continue;
      if(result.dest(i+1, j)<sink)
	result.addTransition(i, j, result.dest(i+1, j));
      else if(result.dest(i+1,j)==sink)
	result.deleteTransition(i, j);
      else
	result.addTransition(i, j, result.dest(i+1, j) - 1);
    }
  }
  result.grow(result.size() - 1, result.alphabetSize());
}
