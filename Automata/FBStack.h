// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file FBStack.h
 * Define a stack of tuples
 */
#ifndef FBSTACK_H_
#define FBSTACK_H_
#include <stack>

class FBTuple {
 public:
  /**Copy constructor*/
  FBTuple(const class FBTuple &other) : f(other.f), 
    child(other.child), result(other.result) {;}
  /**Constructor*/
  explicit FBTuple(const class Formula *g, bool c, bool r) : 
    f(g), child(c), result(r) {;}
  /**Assignment operator*/
  class FBTuple & operator=(const class FBTuple & rhs) {
    f=rhs.f;
    child=rhs.child;
    result=rhs.result;
    return *this;
  }
  const class Formula * f; 
  bool child; 
  bool result;
}; 

typedef std::stack<FBTuple> FBStack;
#endif //FBSTACK_H_
