// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details
// The function buchiRead() is © Marko Mäkelä (marko.makela@hut.fi).

/** @file NonDetAut.C
 * Class for Non-deterministic automaton
 */
#ifdef __GNUC__
#pragma implementation
#endif //__GNUC__
#include <cstdlib>
#include <cstdio>
#include <ctype.h>
#include <list>
#include "NonDetAut.h"
#include "DetAut.h"
#include "TemporalBinOp.h"
#include "TemporalUnOp.h"
#include "Not.h"
#include "BinOp.h"
#include "Const.h"
#include "Atom.h"
#include "BVMap.h"
#include "BVSet.h"
#include "FormulaMap.h"
#include "NumberMap.h"
#include "FBStack.h"

typedef std::list<class BitVector> BVList;
typedef std::list<BVMap::const_iterator> IterList;
typedef std::list<class Formula *> FormulaList;


NonDetAut::NonDetAut(unsigned size, unsigned aSize, unsigned sets) 
  : Automaton(size, aSize, sets), myTransRel(), myInitial(size)
{
  assert(size>0);
  myFinalSets=new unsigned[size]; assert(myFinalSets);
  for(unsigned i=size; i--; ) myFinalSets[i]=0;
}

NonDetAut::~NonDetAut() 
{
  delete[] myFinalSets;
}

void 
NonDetAut::grow(unsigned size, unsigned asize)
{
  assert(size>=mySize && asize>=myAlphabetSize);
  if (size > mySize) {
    unsigned *sets=new unsigned[size]; assert(sets);
    memcpy(sets, myFinalSets, mySize * sizeof *sets);
    memset(sets, 0, (size-mySize) * sizeof *sets);
    delete[] myFinalSets;
    myFinalSets=sets;
    mySize=size;
  }
  if (asize > myAlphabetSize)
    myAlphabetSize=asize;
}





/**Compute the transitive closure w.r.t a label from a set of states
 *@param aut The automaton we are working in
 *@param start The set of states 
 *@param label The label 
 *@param result place holder for the result
 */

inline static void
closure(const class Automaton &aut, const class BitVector &start, unsigned label,
	class BitVector &result)
{
  for(unsigned i=start.getSize(); i--; ) 
    if(start[i]) 
      for (unsigned j=aut.numArcs(i, label); j--; )
	result.assign(aut.dest(i, label, j), true);
}


/**Check if state is a final state
 * @param state bitvector representing the subset defining the state
 * @param finalSets Final set of the automaton
 * @return true iff s\in state: finalSets[s]!=0
 */
inline static bool 
checkFinal(const class BitVector &state, const unsigned* finalSets) 
{
  for(unsigned i=state.getSize(); i--;) {
    if(state[i] && finalSets[i]) return true;
  }
  return false;
}




void 
NonDetAut::determinize(class DetAut &result) const
{
    BVMap statespace;
    unsigned state=0;
    BVMap::const_iterator i=(statespace.insert(BVMap::value_type(myInitial, state))).first;  
    result.setInitial(0);
    IterList work;
    work.push_front(i);
    do {
      BVMap::const_iterator iter=work.front();
      work.pop_front();
      const BitVector &source=(*iter).first;
      for (unsigned j=myAlphabetSize; j--; ) { //loop over all labels
        BitVector dest(mySize);
	closure(*this, source, j, dest);	  
        if (statespace.find(dest)==statespace.end()) { //seen before?
	  if((state+1) >= result.size()) result.grow(result.size()+1, myAlphabetSize);	
	  result.addTransition((*iter).second ,j, ++state);
	  work.push_front((statespace.insert(BVMap::value_type(dest, state))).first);	 
	  if (checkFinal(dest, myFinalSets))
	    result.makeFinal(state);	
	}	
	else
	  result.addTransition((*iter).second ,j, (*statespace.find(dest)).second);
      }    
    }while (!work.empty());
    result.grow(state+1, myAlphabetSize);
}


/**Identify which subformulas belong to rcl(f)
 *@param f formula
 *@param fmap (output) placeholder for subformulas
 *@param rcllist (output)
 *@param apmap (output)
 *@param rcmlmembers (output) bitvector with true iff subformula is in rcllist
 *@return the number elements in rcl(f)
 */


static void rcl(const class Formula &f, FormulaMap &fmap, FormulaList &rcllist, 
		unsigned *apmap, class BitVector &rclmember)
{
  unsigned j=0;
  unsigned k=0;
  for(Formula::PostIterator i=f.newPostIterator(); !i.atEnd(); ++i) {
    if(fmap.find(&(*i))==fmap.end()) {
      if((*i).getType()==Formula::fAtom) {
	apmap[k++]=j;
      }
      (*i).setNum(j); //CHANGED
      fmap.insert(FormulaMap::value_type(&(*i), j++));      
    }
  }

  Formula **rclmap=new Formula*[fmap.size()];
  for(unsigned i=fmap.size(); i--;) { rclmap[i]=0;}
  //identify subformulas belonging to rcl
  bool prevX=false;
  unsigned varid=0;
  for(Formula::Iterator i=f.newIterator(); !i.atEnd(); ++i) {
    Formula::Type type=(*i).getType();
    switch(type) {
    case Formula::fTemporalUnOp:
      rclmap[fmap[&(*i)]]=&(*i);
      if(static_cast<class TemporalUnOp &>(*i).getOp() == TemporalUnOp::Next) {
        prevX=true;
      }
      else { 
        prevX=false;
      }
      break;
    case Formula::fTemporalBinOp:
      prevX=false;
      rclmap[fmap[&(*i)]]=&(*i);
      break;
    case Formula::fConst:
    case Formula::fNot:
    case Formula::fAtom:
    case Formula::fBinOp:
      if (varid==0 || prevX) {
	//if(prevX) {
	rclmap[fmap[&(*i)]]=&(*i);
      } 
      prevX=false;
      break;
    }    
    varid++;
  }
  for(Formula::PostIterator i=f.newPostIterator(); !i.atEnd(); ++i) {
    if(rclmap[fmap[&(*i)]]!=0) {
      rcllist.push_back(&(*i));
      rclmember.assign(fmap[&(*i)] ,true);
    }
  }
  //if(rcllist.empty()) rcllist.push_back(const_cast<class Formula *>(&f));
  delete[] rclmap;
}





/**Check if the set fulfills the condition of the formula 
 * @param f the formula
 * @param set the set of subformulas 
 * @param fmap map from formula to formula index
 * @param apset bitvector with true for each index where ap is at formula index
 * @param rclmember the rcl set
 * @return true iff set is consistent w.r.t. the formula
 */
// static bool sat(const class Formula *f, const class BitVector &set, FormulaMap &fmap, 
// 		class BitVector &apset, const class BitVector & rclmember) 
// {
//   if(apset[fmap[f]]) return true;
//   if(rclmember[fmap[f]]) return set[fmap[f]];  
//   if(set[fmap[f]]) return true;

  
//   Formula::Type type=f->getType();
//   switch(type) {
//   case Formula::fTemporalBinOp:
//   case Formula::fTemporalUnOp:
//   case Formula::fAtom:
//     return false;    
//   case Formula::fBinOp: {
//     const class BinOp *binop=static_cast<const class BinOp *>(f);
//     if(binop->getOp()==BinOp::And) {
//       return sat(binop->getLHS(), set, fmap,apset, rclmember) && 
// 	sat(binop->getRHS(), set, fmap, apset, rclmember);
//     }
//     else {
//       return sat(binop->getLHS(), set, fmap, apset, rclmember) || 
// 	sat(binop->getRHS(), set, fmap, apset, rclmember); 
//     }
//     break;
//   }
//   case Formula::fConst:
//     return (static_cast<const class Const *>(f)->getVal()) ? true : false; 
//   case Formula::fNot: {
//     if(apset[fmap[static_cast<const class Not *>(f)->getOperand()]]) return false;
//     if(rclmember[fmap[static_cast<const class Not *>(f)->getOperand()]]) 
//       return !set[fmap[static_cast<const class Not *>(f)->getOperand()]];  
//     if(set[fmap[static_cast<const class Not *>(f)->getOperand()]]) return false;
//     return true;
//   }

//   }
//   return false;
// }



static bool sat(const class Formula *g, const class BitVector &set, //FormulaMap &fmap, 
		class BitVector &apset, const class BitVector & rclmember) 
{
  if(apset[g->getNum()] || set[g->getNum()]) return true;
  if(rclmember[g->getNum()]) return set[g->getNum()];  

  
  bool result=false;
  FBStack depthStack;
  for (;;) {  //DFS in the formula
    for(;g!=0;) { //go deeper
      if(apset[g->getNum()] || set[g->getNum()]) {
	result=true;
	break;
      }
      if(rclmember[g->getNum()]) {
	result=set[g->getNum()];  
	break;
      }
      Formula::Type type=g->getType();
      switch(type) {
      case Formula::fTemporalBinOp:
      case Formula::fTemporalUnOp:
      case Formula::fAtom:
      result=false;
      g=0;
      break;
      case Formula::fConst:
	result=(static_cast<const class Const *>(g)->getVal()) ? true : false; 
	g=0;
	break;
      case Formula::fNot: 
	depthStack.push(FBStack::value_type(g, true, false));
	g=static_cast<const class Not *>(g)->getOperand();
	break;      
      case Formula::fBinOp: 
	depthStack.push(FBStack::value_type(g, false, false));
	g=static_cast<const class BinOp *>(g)->getLHS();
	break;      	
      }
    }
    for(;;) { //backtrack
      if(depthStack.empty()) {	
	return result;
      }
      struct FBTuple & tuple=depthStack.top(); 
      if(!tuple.child) {
	assert(tuple.f->getType() == Formula::fBinOp);
	tuple.child=true;
	tuple.result=result;
	g=static_cast<const class BinOp *>(tuple.f)->getRHS();
	break;
      }      
      switch(tuple.f->getType()) {
      case Formula::fNot:
	result=!result;
	break;
      case Formula::fBinOp:
	if(static_cast<const class BinOp *>(tuple.f)->getOp()==BinOp::And) {
	  result = result && tuple.result;
	}
	else {
	  result = result || tuple.result;
	}
	break;	
      default:
	assert(false);
      }
      g=tuple.f;
      depthStack.pop();
    }
  }
  return result;
}


/**Set the correct bits in the state corresponding to the given subset of ap:s
 *@param apmap mapping from ap number to formula index 
 *@param num Number of atomic propositions in the formula
 *@param state (output) The state
 *@param ap the number of the subset
 */
static void setAP(const unsigned *apmap, unsigned num, class BitVector &state, unsigned ap)
{
  unsigned j=1;
  for(unsigned i=0; i<num; i++) {
    if(ap & j) { //check if j:th ap should be in //right order
      state.assign(apmap[i], true);
      //fprintf(stderr, "AP %u\n", apmap[i]);
    }
    j=j<<1; //set mask for next bit
  }
  
}


NonDetAut* 
NonDetAut::create(const class Formula &f) 
{
  /**List of states*/
  BVList bvlist;
  /**Map from BV (state) to state number*/
  BVMap bvmap;
  /**Map from formula to formula index*/
  FormulaMap fmap;
  /**RCL Subset of formulas*/
  FormulaList rcllist;  
   /**The rcl subset of formulas*/
  class BitVector rclmember(count(f));
 /**Transition relation for automaton to be built*/
  TransRel transrel;
  /**The number of atomic propositions in the formula*/
  unsigned num=numAP(f);
  /**Map from ap number to formula index*/
  unsigned *apmap=new unsigned[num];

  rcl(f, fmap, rcllist, apmap, rclmember);
 
   
  unsigned count=0;
  class BitVector falseset(fmap.size());
  class BitVector state(fmap.size());
  bvlist.push_front(state);
  bvmap.insert(BVMap::value_type(state, count++));
  while (!bvlist.empty()) {   
    state=bvlist.front();
    bvlist.pop_front();
    for (unsigned i=1<<num; i--; ) {
      class BitVector newstate(fmap.size());
      class BitVector apset(fmap.size());
      setAP(apmap, num, apset, i);
      assert(bvmap.find(state)!=bvmap.end());
      unsigned source=bvmap[state];
      for(FormulaList::const_iterator iter=rcllist.begin(); iter!=rcllist.end(); ++iter) {
	Formula::Type type=(*iter)->getType();
	switch(type) {
	case Formula::fTemporalBinOp: {
	  const class TemporalBinOp * formula=
	    static_cast<const class TemporalBinOp *>(*iter);
	  if(formula->getOp()==TemporalBinOp::Until) {
	    if(sat(formula->getRHS(), newstate, apset, rclmember) || 
	       (sat(formula->getLHS(), newstate, apset, rclmember) && 
		sat(formula, state, falseset, rclmember)))
	      newstate.assign(formula->getNum(), true);
	  }
	  else if (formula->getOp()==TemporalBinOp::Release) {
	    if(sat(formula->getRHS(), newstate, apset, rclmember) && 
	       (sat(formula->getLHS(), newstate, apset, rclmember) || 
		sat(formula, state, falseset, rclmember)))
	      newstate.assign(formula->getNum(), true);
	  }
	  break;
	}
	case Formula::fTemporalUnOp: {
	  const class TemporalUnOp * formula=
	    static_cast<const class TemporalUnOp *>(*iter);
	  if(formula->getOp()==TemporalUnOp::Globally) {
	    if(sat(formula, state, falseset, rclmember) && 
	       sat(formula->getOperand(), newstate, apset, rclmember))
	      newstate.assign(formula->getNum(), true);
	  }
	  else if (formula->getOp()==TemporalUnOp::Finally) {
	    if(sat(formula->getOperand(), newstate, apset, rclmember) || 
	       sat(formula, state, falseset, rclmember))
	      newstate.assign(formula->getNum(), true);	    
	  }
	  else if (formula->getOp()==TemporalUnOp::Next) {
	    if (sat(formula->getOperand(), state, falseset, rclmember)) 
	      newstate.assign(formula->getNum(), true);
	  }
	  break;
	}
	case Formula::fBinOp: {
	  const class BinOp * formula=
	    static_cast<const class BinOp *>(*iter);
	  if(formula->getOp()==BinOp::And) {
	    if(sat(formula->getLHS(), newstate, apset, rclmember) && 
	       sat(formula->getRHS(), newstate, apset, rclmember))
	      newstate.assign(formula->getNum(), true);
	  }
	  else if (formula->getOp()==BinOp::Or) {
	    if(sat(formula->getLHS(), newstate, apset, rclmember) || 
	       sat(formula->getRHS(), newstate, apset, rclmember))
	      newstate.assign(formula->getNum(), true);
	  }
	  break;
	}
	case Formula::fNot: {
	  const class Not * formula=static_cast<const class Not *>(*iter);	  
	  if(!sat(formula->getOperand(), newstate, apset, rclmember)) 
	    newstate.assign(formula->getNum(), true);
	  }
	  break;
	case Formula::fAtom:
	  if(sat(*iter, newstate, apset, rclmember)) {
	    newstate.assign((*iter)->getNum(), true);
	  }
	  break;
	case Formula::fConst: 
	  if(static_cast<const class Const *>(*iter)->getVal()) {
	    newstate.assign((*iter)->getNum(), true);
	  }
	  break;
	}
      }
      std::pair<BVMap::iterator, bool> p=bvmap.insert(BVMap::value_type(newstate, count));
      if(p.second) {
	bvlist.push_back(newstate);
	count++;
      }
      transrel.insert(TransRel::value_type(UIPair((*p.first).second,i) , source));
    }
  }
  class NonDetAut *result=new NonDetAut(count, 1<<num, 1);
  for(TransRel::const_iterator i=transrel.begin(); i!=transrel.end(); ++i) {
    result->addTransition((*i).first.state,(*i).first.letter, (*i).second);
  }
  for(BVMap::const_iterator i=bvmap.begin(); i!=bvmap.end(); ++i) {
    if(sat(&f, (*i).first, falseset, rclmember)) //should false set be ap?
      result->setInitial((*i).second);
  }
  if(result->isInitial(result->getInitial())) //check if an initial state exists
    result->makeFinal(0, 0);  
  delete[] apmap;
  return result;
}

/**Parse a propositional formula from an input stream. The formula
 * is expected to be in post-fix notation.
 * @param input the input stream
 * @return formula parsed from the stream
 */

static Formula *parseGate(FILE *input) 
{
  int ch;
  //eat whitespace
  while(isspace(ch=fgetc(input)));
  switch(ch) {
  case 'i':
  case 'e':
  case '&':
  case '|':
    if(class Formula *left=parseGate(input)) {
      if(class Formula *right=parseGate(input)) {
        switch(ch) {
        case 'i':
          return new class BinOp(BinOp::Impl, left, right);
        case 'e':
          return new class BinOp(BinOp::Equiv, left, right);
        case '&':
          return new class BinOp(BinOp::And, left, right);
        case '|':
          return new class BinOp(BinOp::Or, left, right);
        }
      }
      left->destroy();
    }
    return 0;
  case '!':
    if(class Formula *operand=parseGate(input)) {
      if(ch=='!') {
        return new class Not(operand);
      }
    }
    return 0;
  case 't':    
  case 'f':
    return new class Const(ch=='t');
  case 'p':    
    {     
      unsigned num;
      if (1 != fscanf (input, "%u", &num))
        fputs ("Error in proposition number.\n", stderr);
      else
        return new class Atom(num);
    }
  case EOF:
    fprintf(stderr, "Parse error. Unexpected end of file.\n");
    return 0;
  default:
    fprintf (stderr, "Parse error. Illegal character %c\n ", ch);
    return 0;
  }
}


#define error(msg) fputs(msg, stderr); fclose(autfile); remove(outputFileName); delete aut; return 0;

  /**Pseudo-constructor for the class. The method invokes an external translator
   * and by sending the formula to stdout and reading the result of the invocation
   * from stdin. The formula is output the scheck format and the automaton read is
   * also assumed to be in scheck format. Note that this is not a member function.
   * @param translator path to the external translator
   * @param formula 
   * @return A (generalised) Büchi automaton corresponding to the formula
   */


NonDetAut *
buchiread(const char *translator,  const class Formula &f)
{

  char inputFileName[L_tmpnam+1];
  char outputFileName[L_tmpnam+1];

  if(tmpnam(inputFileName)==0 || tmpnam(outputFileName)==0) {
    fputs("Could not create temporary files.", stderr);
    return 0;
  }


  FILE *formulafile=fopen(inputFileName, "w+");
  if(formulafile==NULL) {
    fclose(formulafile);
    fputs("Could not open temporary files!\n", stderr); 
    return 0;    
  } 
  //print formula to file
  f.print(formulafile);
  fclose(formulafile);

  //create commanline
  char *commandline=new char[strlen(translator)+2*(L_tmpnam+1)+2]; commandline[0]='\0';
  strcat(commandline, translator); strcat(commandline, " "); strcat(commandline, inputFileName);
  strcat(commandline, " "); strcat(commandline, outputFileName);

  int errorcode=system(commandline);
  delete[] commandline;
  remove(inputFileName);

  if (errorcode != 0) {
    fprintf(stderr, "Invocation of the translator failed with error code %d.\n", errorcode);
  }

  FILE *autfile=fopen(outputFileName, "r");
  if(autfile==0) {
    fputs("Could not open temporary files!\n", stderr); 
    return 0;       
  }

  unsigned numStates=0; unsigned numSets=0;
  if(2!=fscanf(autfile, "%u%u", &numStates, &numSets)) {
    fputs("Parse error", stderr);
    fclose(autfile);
    remove(outputFileName);
    return 0;
  }
  if(!numStates) {
    fputs("Empty automaton\n", stderr);
    fclose(autfile); remove(outputFileName); 
    return 0;      
  }

  //initialise mapping from ap_id -> ap_num
  NumberMap APMap;
  FormulaMap fmap;
  unsigned j=0;
  for(Formula::PostIterator i=f.newPostIterator(); !i.atEnd(); ++i) {
    if((*i).getType()==Formula::fAtom) {
      if(fmap.find(&(*i))==fmap.end()) {
	APMap.insert(NumberMap::value_type(static_cast<const class Atom &>(*i).getId(), j++));      
	fmap.insert(FormulaMap::value_type(&(*i), 0));
      }
    }
  }
  fmap.clear();
     
  unsigned numap=numAP(f);
  NonDetAut *aut=new NonDetAut(numStates, 1<<numap, numSets ? numSets : numSets+1);
  if(!numSets) for(unsigned i=numStates; i--;) aut->makeFinal(i,0);
  //read transiton information
  unsigned theInitial=UINT_MAX;
  NumberMap StateMap; //map: state -> new state nr
  NumberMap SetMap;  //map: set -> new set nr 
  int ch;
  for(unsigned i=numStates; i--;) {
    unsigned source, initial;
    source=initial=0;
    //int code=fscanf (autfile, "%u%u", &source, &initial);
    if (2 != fscanf (autfile, "%u%u", &source, &initial) || initial > 1) {
      error("Parse error\n");
    }
    if(StateMap.find(source)==StateMap.end()) {
      if(StateMap.size()==numStates) {
	fprintf(stderr, "Too many states seen by state %u\n", source);
	error(" ");
      }
      StateMap.insert(NumberMap::value_type(source, StateMap.size()));
    }

    if(initial) {
      if(theInitial != UINT_MAX) {
	error("Redefinition of the initial state.");
      }
      theInitial=StateMap[source];
    }

    //read set information
    for(;;) {
      unsigned setnum;
      while (isspace(ch=fgetc(autfile)));
      if(ch=='-') {
	if (1 != fscanf (autfile, "%u", &setnum) || 1 != setnum) {
          error("unexpected data after '-'\n");
        }
        break;
      }
      ungetc(ch, autfile);
      if(!isdigit(ch) || 1 != fscanf(autfile, "%u", &setnum) ) {
	error("Parse error");
      }
      if(SetMap.find(setnum) == SetMap.end()) { //the set has not been seen previously
	if(SetMap.size()==numSets) {
	  fprintf(stderr, "Too many sets seen by state %u and set %u.\n", source, setnum);
	  error(" ");
	}	
	aut->makeFinal(StateMap[source], SetMap.size());
	SetMap.insert(NumberMap::value_type(setnum, SetMap.size()));  
      }
      else {
	aut->makeFinal(StateMap[source], SetMap[setnum]);
      }
    }
    //read transition information
    for(;;) {
      unsigned dest;
      while (isspace(ch=fgetc(autfile)));
      if(ch=='-') {
	if (1 != fscanf (autfile, "%u", &dest) || 1 != dest) {
          fputs ("Unexpected data after '-'\n", stderr);
	  error(" ");
	  return 0;
        }
        break;
      }
      ungetc(ch, autfile);
      if(!isdigit(ch) || 1 != fscanf(autfile, "%u", &dest) ) {
	error("Parse error");
      }
      if(StateMap.find(dest) == StateMap.end()) { //the state has not been seen previously
	if(StateMap.size()==numStates) {
	  fprintf(stderr, "Too many states seen by state %u and transition to %u.\n", source, dest);
	  error("");
	}	
	StateMap.insert(NumberMap::value_type(dest, StateMap.size()));  
      }
      Formula *gate=parseGate(autfile);
      if(!gate) {error(" "); }
      for (unsigned label=1<<numap; label--; ) {
	if(gate->eval(APMap, label))
	  aut->addTransition(StateMap[source], label, StateMap[dest]);
      }
      gate->destroy();
    }    
  }

  while(isspace(ch=fgetc(autfile)));
  if(ch!=EOF) { 
    error("Extraneous non-whitespace data at end of input\n");
    return 0;
  }
  aut->setInitial(theInitial);
  fclose(autfile);
  remove(outputFileName);
  return aut;
}


