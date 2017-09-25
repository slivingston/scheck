// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file main.C
 * Main program and option parsing
 */



#include <cstdio>
#include <getopt.h>
#include "Formula.h"
#include "FormulaAlgs.h"
#include "FormulaSet.h"
#include "NonDetAut.h"
#include "DetAut.h"
#include "Pathologic.h"
#include "PrintAut.h"

static void printHelp()
{
  fputs("Usage: scheck [options] {inputfile}\n", stderr);
  fputs("Options: \n", stderr);
  fputs("-o file\t specify outputfile\n", stderr);
  fputs("-d \t produce a deterministic automaton\n", stderr);
  fputs("-s \t check for syntactic safety\n", stderr);
  fputs("-p translator \t check if formula is pathologic\n", stderr); 
  fputs("-v \t print version number and exit\n", stderr);
  return;
}

struct options {
  /**Flag for checking syntactic safety*/
  bool syntactic;  
  /**Flag for checking pathologic safety*/
  bool pathologic;
  /**Flag for requesting a deterministic automaton*/
  bool deterministic;
  /**Flag for requesting version*/
  bool version;
};


int main(int argc, char **argv)
{
  /**Track error code*/
  int error=0;
  /**Variables defined by getopt*/
  extern int optind;
  extern char *optarg;
  /**Filename of external translator*/
  char *translator=0;
  /**Pointer to input and output file*/
  FILE *inputfile=NULL;
  FILE *outputfile=NULL;
  /**Structure to store option flags*/
  struct options opt = {false, false, false};

  /**parse options*/
  while(!error) {
    int c=getopt(argc, argv, "Fvdsp:o:");
    if (c==-1) break; //no more options
    switch(c) {
    case 'v':
      opt.version=true;
      break;
    case 'o': //open output file
      outputfile=fopen(optarg, "w");
      if(outputfile==NULL) {
	fprintf(stderr, "Could not open file %s.\n", optarg);
	error=-1;
      }
      break;    
    case 's':
      opt.syntactic=true;
      break;
    case 'p':
      opt.pathologic=true;
      translator=new char[strlen(optarg)+1];
      strcpy(translator, optarg);
      break;
    case 'F':
      //dummy case
      break;
    case 'd':
      opt.deterministic=true;
      break;
    case '?':      
      printHelp();
      error=1;
      break;
    }
  }
  if(error) return error;
  if(opt.version) {
    fputs("scheck version 1.2.0.\n©Timo Latvala (timo.latvala@hut.fi) 2004.\n", stderr);
    return 0;
  }

  if(optind < argc) { //open input file
    inputfile=fopen(argv[optind], "r");
    if(inputfile==NULL) {
      fprintf(stderr, "Could not open file %s for reading.\n", argv[optind]);
      return -1;
    }
    if(++optind < argc) {
      fputs("Too many arguments.\n", stderr);
      printHelp();
      return -1;
    }
  }
  else {
    inputfile=stdin;
  }
  
  if(outputfile==NULL) {
    outputfile=stdout;
  }
  
  class Formula *f=0;
  if(!(f=parseFormula(inputfile))) return -1;
 

  class Formula *f2=removeDerived(f);
  f->destroy();      
  class Formula *f4=toNNF(f2);
  f2->destroy();
  if(opt.syntactic && !isSyntacticSafe(*f4)) {
    fputs("Given formula is not syntactically safe.\n", stderr);
    error=-1;
    f4->destroy();
  } 

  if(!error) {
   class Formula *f5=rewriteFormula(f4);    
    while(!(*f4==*f5)) {
      f4->destroy();
      f4=f5;
      f5=rewriteFormula(f4);    
    }
    f4->destroy();
    class Formula *f3=dagify(*f5); 
    f5->destroy();
    Automaton *aut=NonDetAut::create(*f3);
    if(opt.deterministic || opt.pathologic) {
      DetAut result(1, aut->alphabetSize(), 1);
      static_cast<class NonDetAut *>(aut)->determinize(result);
      delete aut;
      DetAut *res=result.minimise();
      if(opt.pathologic) {
	Pathologic pathologic(*f3, *res, translator);
	if(!pathologic.pathologic()) {
	  error=1;
	  fputs("The formula is pathologic!\n", stderr);
	}
	delete[] translator;
      }
      removeSink(*res);
      aut=res;
    }
    if(!error) printLabelAut(outputfile, *aut, *f3);
    FormulaSet fset;
    for(Formula::PostIterator i=f3->newPostIterator(); !i.atEnd(); ++i) {
      if(fset.find(&(*i))!=fset.end()) {
	fset.insert(&(*i));
      }
    }
    for(FormulaSet::iterator i=fset.begin(); i!=fset.end(); ++i)
      delete *i;
    delete aut;
  }
  fclose(inputfile); fclose(outputfile);
  return error;
}
