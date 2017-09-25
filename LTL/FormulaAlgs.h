// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file FormulaAlgs.h
 * Header file for algorithms related to formula manipulation
 */
#ifndef FORMULAALGS_H_
#define FORMULAALGS_H_
/**Check if a formula is a syntactic safety formula*/
bool 
isSyntacticSafe(const class Formula &f);

/**@return A syntactically equal formula which shares substructure*/
class Formula *
dagify(const class Formula &formula);

/**@return An equivalent formula which is negation normal form*/
class Formula *
toNNF(const class Formula *f);

/**@return An equivalent formula without some of the derived operators*/
class Formula *
removeDerived(const class Formula *f); 

/**Create a formula from an character stream describing the formula in postfix notation
 *@param inputFile  The input stream
 *@retrun The formula*/
class Formula * 
parseFormula(FILE *inputFile);

/**Use rewrite rules to optimise the formulas for automata conversion
 * @param f formula to be processed
 * @return an equivalent formula
 */
class Formula *
rewriteFormula(const class Formula *f);
#endif //FORMULAALGS_H_
