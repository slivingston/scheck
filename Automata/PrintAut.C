// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file PrintAut.C
 * Functions related to printing an automaton
 */
#include "PrintAut.h"
#include "Implicant.h"
#include "Automaton.h"
#include "Formula.h"
#include "Atom.h"
#include "FormulaSet.h"
#include "MultiMap.h"
#include "Implicant.h"
#include <cstdio>
#include <set>
typedef std::set<class Implicant> ImplicantSet;



/**A helper function to QM-algorithm to compute a (minimal) covering given a set of implicants
 * @param iset The set of terms to cover
 * @param covering (output) Set of implicants covering the terms
 */
static void cover(MultiMap::const_iterator iter, unsigned count, ImplicantSet &covering)
{
  unsigned *covered=new unsigned[count];
  for(unsigned i=count; i--; ) covered[i]=0;

  for(ImplicantSet::iterator i=covering.begin(); i!=covering.end(); ++i) { 
    unsigned k=0;
    //compute terms covered by *i
    for(MultiMap::const_iterator j=iter; k<count; ++j, ++k) {
      if((*i).covers((*j).second)) 
	++covered[k];
    }
  }
  //check if *i can be deleted (its terms are covered by other implicants)
  for(ImplicantSet::iterator i=covering.begin(); i!=covering.end();) { 
    bool del=true;
    unsigned k=0;
    for(MultiMap::const_iterator j=iter; k<count; ++j, ++k) {  
      if((*i).covers((*j).second) && covered[k]==1) { // *i is necessary (only implicant to cover a term)
	del=false;
	break;      
      }
    }
    if(del) { //delete unnecessary implicant
      unsigned k=0;
      ImplicantSet::iterator it=i++;
      for(MultiMap::const_iterator j=iter; k<count; ++j, ++k) { //check which implicants it covers
	if((*it).covers((*j).second)) { 
	  --covered[k];
	}
      }
      covering.erase(it);
    }
    else {
      ++i;
    }
  } 
  delete[] covered;  
  return;    
}

/**Given a Boolean expression as a SOP, find a (nearly) optimial set of 
 * covering implicants which cover the terms in the SOP-expression.
 * @param iter Iterator pointing the first term in the SOP-expression
 * @param count Number of terms in the SOP-expression.
 * @param num Number of atoms in the terms
 * @param covering (output) The set of covering implicants
 */

static void QM(MultiMap::const_iterator iter, unsigned count, unsigned num, ImplicantSet &covering) 
{
  ImplicantSet iset;
  unsigned k=0;
  for(MultiMap::const_iterator i=iter; k<count; ++i, ++k) {
    iset.insert(Implicant((*i).second, num));
  }
  
  while(!iset.empty()) {
    for(ImplicantSet::iterator i=iset.begin(); i!=iset.end(); ++i) {
      bool joined=false;
      for (ImplicantSet::iterator j=i; j!=iset.end(); ++j) { 
	if((*i).joinable(*j)) {
	  Implicant temp(*i);
	  temp.join(*j);
	  iset.insert(temp);
	  iset.erase(j);
	  joined=true;
	  break;
	}
      }
      if(!joined) {
	covering.insert(*i);
      }
      ImplicantSet::iterator it=i;
      iset.erase(it);
    }
  } 
  cover(iter, count, covering);
  return;
}

/**Print a label to a stream
 * @param stream 
 * @param iter iterator pointing to first arc
 * @param count the number of consequetive arcs with the same destination
 * @param apid mapping from formula index to ap index
 * @param apindex mapping from ap number to formula index
 * @param num Number of atomic propositions in formula
 */

static void
printLabel(FILE * stream, MultiMap::const_iterator &iter, unsigned count, const unsigned *apid, 
	   const unsigned *apindex, const unsigned num) {
  /**Check for the special case of all labels to a single state*/
  if(count == 1u<<num) {
    fputs(" t\n", stream);
    while(count--) ++iter;
    return;    
  }
  
  ImplicantSet covering;
  QM(iter, count, num, covering);

  unsigned k=covering.size();
  for(ImplicantSet::const_iterator i=covering.begin(); i!=covering.end(); ++i,k--) {
    if(k>1) fputs(" | ", stream);
    unsigned numNotDC=0;
    for (unsigned j=(*i).size(); j--; ) {
      if((*i)[j]!=Implicant::DC) {
	numNotDC++;
      }
    }
    for(unsigned j=(*i).size(); j--; ) {
      if((*i)[j]==Implicant::DC) continue;
      if (numNotDC>1) fputs(" & ", stream);      
      if ((*i)[j]==Implicant::True) {
	fprintf(stream, " p%u", apid[apindex[j]]);
	numNotDC--;
      }
      else if ((*i)[j]==Implicant::False){
	fprintf(stream, " ! p%u", apid[apindex[j]]);
	numNotDC--;
      }
    }
    assert(numNotDC==0);    
  }
  assert(k==0);
  fputs("\n", stream);
  while(count--) ++iter;
  return; 
}


/**Print a label to a stream
 * @param stream 
 * @param iter iterator pointing to first arc
 * @param count the number of consequetive arcs with the same destination
 * @param apid mapping from formula index to ap index
 * @param apindex mapping from ap number to formula index
 * @param num Number of atomic propositions in formula
 */
// static void
// printLabel(FILE * stream, MultiMap::const_iterator &iter, unsigned count, const unsigned *apid, 
// 	   const unsigned *apindex, const unsigned num) {
//   /**Check for the special case of all labels to a single state*/
//   if(count == 1u<<num) {
//     fputs(" t\n", stream);
//     while(count--) ++iter;
//     return;    
//   }

//   for(unsigned k=count; k--; ) {
//     if(k>0) fputs(" | ", stream);
//     /**mask for label*/
//     unsigned j=1;
//     for (unsigned i=0; i<num; i++) {
//       if (i<num-1) {
// 	fputs(" & ", stream);
//       }
//       if ((*iter).second & j) {
// 	fprintf(stream, " p%u", apid[apindex[i]]);
//       }
//       else {
// 	fprintf(stream, " ! p%u", apid[apindex[i]]);
//       }
//       j=j<<1;    
//     }
//     ++iter;
//   }
//   fputs("\n", stream);
// }

/**Function for initilialising apid and a formulaset
 * @param apindex (output) map from ap number to formula index
 * @param apid (output) map from formula index to ap id
 */
static void initAPI(unsigned *apindex, unsigned *apid, const class Formula &f) 
{
  unsigned index=0;
  unsigned counter=0;
  FormulaSet fset;
  /**init apindex and apid*/
  for(class Formula::PostIterator i=f.newPostIterator(); !i.atEnd(); ++i) {
    if(fset.find(&(*i))==fset.end()) {
      if((*i).getType()==Formula::fAtom) {
	apindex[index++]=counter;
	apid[counter]=static_cast<const Atom &>(*i).getId();
      }
      else {
	apid[counter]=0;
      }
      counter++;
      fset.insert(&(*i));
    }
  }  
  return;
}

#ifdef NORMAL
/**Print an automaton with labels to a stream
 * @param stream 
 * @param automaton Automaton to be printed
 * @param f Formula used to construct the automaton
 */
void
printLabelAut(FILE * stream, const class Automaton & automaton, const class Formula &f)
{
  /**Number of atomic propositions*/
  unsigned apnum=numAP(f);
  /**Map from ap number to formula index */
  unsigned *apindex=new unsigned[apnum];
  /**Map from formula index to ap id*/
  unsigned *apid= new unsigned[count(f)];

  initAPI(apindex, apid, f);
  MultiMap arcs;
  fprintf(stream, "%u %u\n", automaton.size(), automaton.getNumSets());    
  for(unsigned state=automaton.size(); state--;) {
    fprintf(stream, "%u", state); (automaton.isInitial(state)) ? fputs(" 1 ", stream) : 
      fputs(" 0 ", stream); 
    unsigned set=0;
    (automaton.isFinal(state, set)) ? fprintf(stream, "%u -1\n", set) : fputs("-1 \n", stream); 

    for(unsigned label=automaton.alphabetSize(); label--; ) {
      for(unsigned k=automaton.numArcs(state,label); k--; ) {
	arcs.insert(MultiMap::value_type(automaton.dest(state,label,k), label));
      }
    }    
    for(MultiMap::const_iterator i=arcs.begin(); i!=arcs.end(); ) {      
      fprintf(stream, "%u", (*i).first);
      printLabel(stream, i, arcs.count((*i).first), apid, apindex, apnum);	
    }
    arcs.clear();
    fputs("-1\n", stream);
  }  

  delete[] apid;
  delete[] apindex;
  return;
}
#endif //NORMAL


#ifdef MARIA
void
printLabelAut(FILE *stream, const class Automaton &automaton, const class Formula &f)
{
  /**Number of atomic propositions*/
  unsigned apnum=numAP(f);
  /**Map from ap number to formula index */
  unsigned *apindex=new unsigned[apnum];
  /**Map from formula index to ap id*/
  unsigned *apid= new unsigned[count(f)];
 
  initAPI(apindex, apid, f);
  MultiMap arcs;
  fprintf(stream, "%u", automaton.size()); fputs("0\n", stream);
  for(unsigned state=automaton.size(); state--;) {
    fprintf(stream, "%u", state); (automaton.isInitial(state)) ? fputs(" 1 ", stream) : 
      fputs(" 0 ", stream); 
    unsigned set=0;
    (automaton.isFinal(state, set)) ? fprintf(stream, "%u -1\n", set) : fputs("-1 \n", stream); 
    for(unsigned label=automaton.alphabetSize(); label--; ) {
      for(unsigned k=automaton.numArcs(state,label); k--; ) {
	arcs.insert(MultiMap::value_type(automaton.dest(state,label,k), label));
      }
    }    
    for(MultiMap::const_iterator i=arcs.begin(); i!=arcs.end(); ) {      
      fprintf(stream, "%u", (*i).first);
      printLabel(stream, i, arcs.count((*i).first), apid, apindex, apnum);	
    }
    arcs.clear();
    fputs("-1\n", stream);
  }  

  delete[] apid;
  delete[] apindex;
  return;
} 
#endif //MARIA

#ifdef SPIN
static void
printSpinLabel(FILE * stream, MultiMap::const_iterator &iter, unsigned count, 
	       const unsigned *apid,  const unsigned *apindex, const unsigned num) {
  /**Check for the special case of all labels to a single state*/
  if(count == 1u<<num) {
    fputs("(1)", stream);
    while(count--) ++iter;
    return;    
  }

  //compute the covering
  ImplicantSet covering;
  QM(iter, count, num, covering);
 
  //print the label
  fputs("(", stream);
  unsigned k=covering.size();
  for(ImplicantSet::const_iterator i=covering.begin(); i!=covering.end(); ++i,k--) {
    unsigned numNotDC=0;
    for (unsigned j=(*i).size(); j--; ) {
      if((*i)[j]!=Implicant::DC) {
	numNotDC++;
      }
    }
    fputs("(", stream);
    for(unsigned j=(*i).size(); j--; ) {
      if((*i)[j]==Implicant::DC) continue;
      if ((*i)[j]==Implicant::True) {
	fprintf(stream, "p%u", apid[apindex[j]]);
	numNotDC--;
      }
      else if ((*i)[j]==Implicant::False){
	fprintf(stream, "! p%u", apid[apindex[j]]);
	numNotDC--;
      }
      if (numNotDC>0) fputs(" && ", stream);      
    }
    fputs(")", stream);
    if(k>1) fputs(" || ", stream);    
    assert(numNotDC==0);    
  }
  assert(k==0);
  fputs(")", stream);
  while(count--) ++iter;
  return;
}

void 
printLabelAut(FILE *stream, const class Automaton &aut, const class Formula &f)
{
  /**Number of atomic propositions*/
  unsigned apnum=numAP(f);
  /**Map from ap number to formula index */
  unsigned *apindex=new unsigned[apnum];
  /**Map from formula index to ap id*/
  unsigned *apid= new unsigned[count(f)];
 
  initAPI(apindex, apid, f);

  fputs("never {\n", stream);
  for(unsigned state=0; state< aut.size(); state++) {
    unsigned set=0;
    if(aut.isInitial(state)) {
      fprintf(stream, "T%u_init:\n", state);
    }
    else if (aut.isFinal(state, set)) {
      fprintf(stream, "accept_S%u:\n", state);
      fputs("\t assert(0)\n", stream);
    }
    else {
      fprintf(stream, "T0_S%u:\n", state);
    }
    MultiMap arcs;
    if(!aut.isFinal(state, set)) {
      fputs("\t if\n", stream);
      for(unsigned label=aut.alphabetSize(); label--; ) {
	for(unsigned k=aut.numArcs(state,label); k--; ) {
	  arcs.insert(MultiMap::value_type(aut.dest(state,label,k), label));
	}
      }    
      for(MultiMap::const_iterator i=arcs.begin(); i!=arcs.end(); ) {      
	unsigned dest=(*i).first;
	fputs("\t :: ", stream);
	printSpinLabel(stream, i, arcs.count(dest), apid, apindex, apnum);	
	fputs(" -> goto ", stream);
	if(aut.isInitial(dest)) {
	  fprintf(stream, "T%u_init\n", dest);
	}
	else if(aut.isFinal(dest, set)) {
	  fprintf(stream, "accept_S%u\n", dest);
	}
	else {
	  fprintf(stream, "T0_S%u\n", dest);
	}
      }
      arcs.clear();
      fputs("\t fi;\n", stream);
    }
  }
  fputs("}\n", stream);
}
#endif //SPIN
