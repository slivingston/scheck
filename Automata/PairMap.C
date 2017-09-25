// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file PairMap.C
 * Map a pair of unsigned integers to an integer
 */

#ifdef __GNUC__
# pragma implementation
#endif // __GNUC__

#include <cstring>
#include "PairMap.h"

PairMap::PairMap(unsigned xsize, unsigned ysize) : myXSize(xsize), myYSize(ysize),
						   myXAllocated(xsize), myYAllocated(ysize)
{
  myMap=new unsigned*[xsize]; assert(myMap);
  for(unsigned i=xsize; i--; ) {
    myMap[i]=new unsigned[ysize]; assert(myMap[i]);
    for(unsigned j=ysize; j--; ) myMap[i][j]=0;    
  }
}

PairMap::~PairMap()
{
  for(unsigned i=myXAllocated; i--; )
    delete[] myMap[i];
  delete[] myMap;
} 

void
PairMap::grow(unsigned xsize, unsigned ysize)
{
  assert(xsize>=myXSize && ysize>=myYSize);
  if(xsize<= myXAllocated) {
    myXSize=xsize;
  }
  else { //xsize > myXAllocated
    while(myXAllocated < xsize) myXAllocated <<= 1;
    unsigned **temp=new unsigned*[myXAllocated]; assert(temp);
    memcpy(temp, myMap, myXSize*sizeof *temp);      
    delete[] myMap;
    myMap=temp;
    for(unsigned i=myXAllocated; i-- > myXSize;) {
      myMap[i]=new unsigned[myYAllocated];
      assert(myMap[i]);
      memset(myMap[i], 0, myYAllocated * sizeof *myMap[i]);
    }
    myXSize=xsize;
  }

  if(ysize <= myYAllocated) {
    myYSize=ysize;
  }
  else { //ysize > myYAllocated
    while(myYAllocated < ysize) myYAllocated <<= 1;
    for(unsigned i=myXSize; i--; ) {
      unsigned *temp=new unsigned[myYAllocated]; assert(temp);
      memcpy(temp, myMap[i], myYSize*sizeof *temp);
      memset(temp+myYSize, 0, (myYAllocated-myYSize)*sizeof *temp);  
      delete[] myMap[i];
      myMap[i]=temp;
    }
    myYSize=ysize;
  }
  return;
}

