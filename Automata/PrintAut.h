// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file PrintAut.h
 * Headers for functions related to printing an automaton
 */
  
#ifndef PRINTAUT_H_
#define PRINTAUT_H_
//forward declaration of file
struct _IO_FILE;
typedef struct _IO_FILE FILE;


/**Print an automaton with labels
 * @param automaton Automaton to be printed
 * @param f Formula used to construct the automaton
 */
void
printLabelAut(FILE *stream, const class Automaton & automaton, const class Formula &f);

#endif //PRINTAUT_H_
