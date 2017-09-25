// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file FormulaAlgs.C
 * Algorithms related to formula manipulation
 */
#include <cstdio>
#include <ctype.h>
#include "Formula.h"
#include "TemporalUnOp.h"
#include "TemporalBinOp.h"
#include "BinOp.h"
#include "Not.h"
#include "Const.h"
#include "Atom.h"
#include "FormulaAlgs.h"
#include "FormulaSet.h"

/**Check if a formula in NNF is a syntactic safety formula
 *@param f Formula to be checked
 *@return true iff the formula is syntacically safe, otherwise false
 */
bool 
isSyntacticSafe(const class Formula &f)
{
  for(class Formula::Iterator iter=f.newIterator(); !iter.atEnd(); ++iter) {
    Formula::Type kind = (*iter).getType();
    switch(kind) {
    case Formula::fTemporalBinOp: {
      const class TemporalBinOp &formula=static_cast<const class TemporalBinOp &>(*iter);
      if (formula.getOp() == TemporalBinOp::Release) return false;
      break;	
    }
    case Formula::fTemporalUnOp: {
      const class TemporalUnOp &formula=static_cast<const class TemporalUnOp &>(*iter);
      if(formula.getOp() == TemporalUnOp::Globally) return false;
      break;
    }
    case Formula::fBinOp:
    case Formula::fAtom: 
    case Formula::fNot: 
    case Formula::fConst:
      break;
    }
  }
  return true;
}



class Formula * parseFormula(FILE *inputFile) 
{
  int ch;
  //eat whitespace
  while(isspace(ch=fgetc(inputFile))); 
  
  switch(ch) {
  case 'U':
  case 'V':
  case 'i':
  case 'e':
  case '&':
  case '|':
    if(class Formula *left=parseFormula(inputFile)) {
      if(class Formula *right=parseFormula(inputFile)) {
	switch(ch) {
	case 'U':
	  return new class TemporalBinOp(TemporalBinOp::Until,  left, right);
	case 'V':
	  return new class TemporalBinOp(TemporalBinOp::Release, left, right);	  
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
  case 'X':
  case 'G':    
  case 'F':
    if(class Formula *operand=parseFormula(inputFile)) {
      switch(ch) {
      case '!':
	return new class Not(operand);
      case 'X':
	return new class TemporalUnOp(TemporalUnOp::Next, operand);	
      case 'G':
	return new class TemporalUnOp(TemporalUnOp::Globally, operand);
      case 'F':
	return new class TemporalUnOp(TemporalUnOp::Finally, operand);
      }
    }
    return 0;
  case 't':    
  case 'f':
    return new class Const(ch=='t');
  case 'p':    
    {	  
      unsigned num;
      if (1 != fscanf (inputFile, "%u", &num))
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
  
/**Return a new formula wher the derived operators have been replaced with 
 * the fundamental operators
 * @param f The formula to be processed
 * @return the new processed formula 
 */


class Formula *
removeDerived(const class Formula *f) 
{
  enum Formula::Type type=f->getType();
  switch(type) {
  case Formula::fTemporalUnOp:
    {
      enum TemporalUnOp::Op op=static_cast<const class TemporalUnOp*>(f)->getOp();
      const class Formula *operand=static_cast<const class TemporalUnOp*>(f)->getOperand();
      //switch(op) {
      //case TemporalUnOp::Finally:
	//return new TemporalBinOp(TemporalBinOp::Until, new Const(true), removeDerived(operand));
      //case TemporalUnOp::Globally: 
	//return new TemporalBinOp(TemporalBinOp::Release, new Const(false), removeDerived(operand));
      //case TemporalUnOp::Next:
	return new TemporalUnOp(op, removeDerived(operand));
    }
  case Formula::fBinOp:
    {
      enum BinOp::Op op=static_cast<const class BinOp*>(f)->getOp();
      const class Formula *lhs=static_cast<const class BinOp*>(f)->getLHS();
      const class Formula *rhs=static_cast<const class BinOp*>(f)->getRHS();
      switch(op) {
      case BinOp::Impl:
	return new BinOp(BinOp::Or, new Not(removeDerived(lhs)), removeDerived(rhs));
      case BinOp::Equiv:
	return new BinOp(BinOp::And, removeDerived(new BinOp(BinOp::Impl, lhs, rhs)),
			 removeDerived(new BinOp(BinOp::Impl, rhs, lhs)));
      case BinOp::And:
      case BinOp::Or:
	return new BinOp(op, removeDerived(lhs), removeDerived(rhs));
      }
    }
  case Formula::fNot:
    return new Not(removeDerived(static_cast<const class Not*>(f) -> getOperand()));
  case Formula::fTemporalBinOp:
    {
      const class TemporalBinOp *formula=static_cast<const class TemporalBinOp*>(f);
      return new TemporalBinOp(formula->getOp(), 
			       removeDerived(formula->getLHS()),
			       removeDerived(formula->getRHS()));   
    }
  case Formula::fAtom: 
    return f->clone();
  case Formula::fConst:
    return f->clone();
  default:
    fputs("Error in removing derived operators. Unknown formula type.\n", stderr);
    exit(-1);
  }
  return 0;
}


/**Convert a formula to negation normal form (NNF). The 
 * derived operators must have been removed in advance!
 * @param f The formula to be converted
 * @return The equivalent NNF
 */

class Formula *
toNNF(const class Formula *f) 
{
  enum Formula::Type type=f->getType(); 
  switch(type) {
  case Formula::fBinOp: {
    enum BinOp::Op op=static_cast<const class BinOp*>(f)->getOp();
    const class BinOp *formula=static_cast<const class BinOp*>(f); 
    switch(op) {
    case BinOp::Impl:	
    case BinOp::Equiv:
      fputs("Error! The derived operators have not been removed!\n", stderr);
      return 0;
    case BinOp::And:
    case BinOp::Or:
      return new BinOp(op, toNNF(formula->getLHS()), toNNF(formula->getRHS()));
    }
  }  
  case Formula::fTemporalBinOp: {
    const class TemporalBinOp *formula=static_cast<const class TemporalBinOp*>(f);
    return new TemporalBinOp(formula->getOp(), toNNF(formula->getLHS()),
			     toNNF(formula->getRHS()));   
  }
  case Formula::fTemporalUnOp:
    {
      //enum TemporalUnOp::Op op=static_cast<const class TemporalUnOp*>(f)->getOp();
      const class TemporalUnOp *formula=static_cast<const class TemporalUnOp*>(f);
      return new TemporalUnOp(formula->getOp(), toNNF(formula->getOperand()));     
    }
  case Formula::fAtom: 
    return f->clone();   // new Atom(static_cast<const class Atom*>(f)->getId());
  case Formula::fConst:
    return f->clone();  //new Const(static_cast<const class Const*>(f)->getVal());
  case Formula::fNot:
    {
      const class Formula *operand=static_cast<const class Not*>(f)->getOperand();
      enum Formula::Type kind=operand->getType();
      switch(kind) {
      case Formula::fBinOp:
	{
	  enum BinOp::Op op=static_cast<const class BinOp*>(operand)->getOp();
	  const class Formula *lhs=static_cast<const class BinOp*>(operand)->getLHS();
	  const class Formula *rhs=static_cast<const class BinOp*>(operand)->getRHS();
	  switch(op) {
	  case BinOp::Impl:	
          case BinOp::Equiv:
	    fprintf(stderr, "Error! The derived operators have not been removed! ");
	    return 0;
	  case BinOp::And:
	    return new BinOp(BinOp::Or, toNNF(new Not(lhs)), toNNF(new Not(rhs))); 
	  case BinOp::Or:
	    return new BinOp(BinOp::And, toNNF(new Not(lhs)), toNNF(new Not(rhs))); 
	    
	  }
	}
      case Formula::fTemporalBinOp:
	{
	  const class Formula *lhs=static_cast<const class TemporalBinOp*>(operand)->getLHS();
	  const class Formula *rhs=static_cast<const class TemporalBinOp*>(operand)->getRHS();
	  if(TemporalBinOp::Until == static_cast<const class TemporalBinOp*>(operand)->getOp()) 
	    return new TemporalBinOp(TemporalBinOp::Release, toNNF(new Not(lhs)), toNNF(new Not(rhs)));
	  else
	    return new TemporalBinOp(TemporalBinOp::Until, toNNF(new Not(lhs)), toNNF(new Not(rhs)));
	}
      case Formula::fTemporalUnOp:
	{
	  enum TemporalUnOp::Op op=static_cast<const class TemporalUnOp*>(operand)->getOp();
	  switch(op) {
	  case TemporalUnOp::Next:
	    return new TemporalUnOp(op, 
				    toNNF(new Not(static_cast<const class TemporalUnOp*>(operand)->getOperand())));
	  case TemporalUnOp::Finally:
	    return new TemporalUnOp(TemporalUnOp::Globally, 
				    toNNF(new Not(static_cast<const class TemporalUnOp*>(operand)->getOperand())));
	  case TemporalUnOp::Globally:
	    return new TemporalUnOp(TemporalUnOp::Finally, 
				    toNNF(new Not(static_cast<const class TemporalUnOp*>(operand)->getOperand())));
	    //fprintf(stderr, "Error! The derived operators have not been removed! ");
	    //return 0;
	  }
	}
      case Formula::fAtom:
	//return new Atom(static_cast<const class Atom*>(operand)->getId(), 
	//		!static_cast<const class Atom*>(operand)->getSign());
	return new Not(operand->clone());
      case Formula::fConst:
	return new Const(!static_cast<const class Const*>(operand)->getVal());
      case Formula::fNot:
	const class Formula *ope = static_cast<const class Not*>(operand)->getOperand();
	return toNNF(ope);
	break;	
      }      
    }
  default:
    fputs("No such temporal operator!\n", stderr);
    exit(-1);
  }
  return 0;
}

/**Make a new formula which shares substructure
 *@param f The formula to process
 *@return The equivalent formula which shares substructure
*/

class Formula *
dagify(const class Formula &f) 
{  
  FormulaSet Table;
  class Formula *temp=0;
  for(class Formula::PostIterator iter=f.newPostIterator(); !iter.atEnd(); ++iter) {
    FormulaSet::iterator p = Table.find(&(*iter));
    enum Formula::Type type=(*iter).getType();
    if(p == Table.end()) {
      switch(type) {      
      case Formula::fTemporalBinOp: 
	temp=new TemporalBinOp(static_cast<class TemporalBinOp &>(*iter).getOp(),
			       *Table.find(static_cast<class TemporalBinOp &>(*iter).getLHS()) , 
			       *Table.find(static_cast<class TemporalBinOp &>(*iter).getRHS()));
	Table.insert(temp);
	break;
      case Formula::fBinOp: 
	temp=new BinOp(static_cast<class BinOp &>(*iter).getOp(), 
		       *Table.find(static_cast<class BinOp &>(*iter).getLHS()), 
		       *Table.find(static_cast<class BinOp &>(*iter).getRHS()));
	Table.insert(temp);
	break;
      case Formula::fTemporalUnOp: 
	temp=new TemporalUnOp(static_cast<class TemporalUnOp &>(*iter).getOp(), 
			      *Table.find(static_cast<class TemporalUnOp &>(*iter).getOperand()));
	Table.insert(temp);
	break;
      case Formula::fNot: 
	temp=new Not(*Table.find(static_cast<class Not &>(*iter).getOperand()));
	Table.insert(temp);
	break;
      case Formula::fAtom: 
	temp=(*iter).clone();
	Table.insert(temp);
	break;
      case Formula::fConst: 
	temp=(*iter).clone();
	Table.insert(temp);
	break;
      }
    } 
  } 
  return temp;
}

class Formula *
rewriteFormula(const class Formula *f) 
{
  switch(f->getType()) {
  case Formula::fNot: 
  case Formula::fAtom: 
  case Formula::fConst: 
    return f->clone();
  case Formula::fTemporalBinOp: {
    const class TemporalBinOp *temp=static_cast<const class TemporalBinOp *>(f); 
    return new TemporalBinOp(temp->getOp(), rewriteFormula(temp->getLHS()), 
			     rewriteFormula(temp->getRHS()));   
  }    
  case Formula::fTemporalUnOp: {
    const class TemporalUnOp *temp=static_cast<const class TemporalUnOp *>(f); 
    switch(temp->getOp()) {
    case TemporalUnOp::Globally:  
    case TemporalUnOp::Finally: {
      if(temp->getOperand()->getType()==Formula::fTemporalUnOp) { 
	const class TemporalUnOp *unop=static_cast<const class TemporalUnOp *>(temp->getOperand());
	if (unop->getOp() == TemporalUnOp::Next) {
	  return new class TemporalUnOp(TemporalUnOp::Next, 
					new class TemporalUnOp(temp->getOp(), 
							       rewriteFormula(unop->getOperand())));
	}
      }
      break;
    }      
    case TemporalUnOp::Next:
      break;
    }
    return new TemporalUnOp(temp->getOp(), rewriteFormula(temp->getOperand()));
  }
  case Formula::fBinOp: {
    const class BinOp *temp=static_cast<const class BinOp *>(f); 
    if((temp->getLHS()->getType() == Formula::fTemporalUnOp) &&
       (temp->getLHS()->getType() == temp->getRHS()->getType())) { //rewrite rule potentially applies
      const class TemporalUnOp *lhs=static_cast<const class TemporalUnOp *>(temp->getLHS()); 
      const class TemporalUnOp *rhs=static_cast<const class TemporalUnOp *>(temp->getRHS());
      if(lhs->getOp() != rhs->getOp()) 
	return new BinOp(temp->getOp(), rewriteFormula(temp->getLHS()), 
			 rewriteFormula(temp->getRHS())); 
      class BinOp binop(temp->getOp(), lhs->getOperand(), rhs->getOperand());
      switch(lhs->getOp()) {
      case TemporalUnOp::Globally:
	if(temp->getOp() != BinOp::And) break;
	//binop is intentionally a stack object so that it will be deallocated
	return new TemporalUnOp(lhs->getOp(), rewriteFormula(&binop));
      case TemporalUnOp::Finally:
	if(temp->getOp() != BinOp::Or) break;
	return new TemporalUnOp(lhs->getOp(), rewriteFormula(&binop));
      case TemporalUnOp::Next:
	return new TemporalUnOp(lhs->getOp(), rewriteFormula(&binop)); 
				
      }
    }
    return new BinOp(temp->getOp(), rewriteFormula(temp->getLHS()), 
		     rewriteFormula(temp->getRHS()));        
  } 
  }
  return 0;
}

