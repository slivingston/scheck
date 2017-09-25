// ©2003 Timo Latvala (timo.latvala@hut.fi). See the file COPYING for details

/** @file NumberMap.h
 * Define a mapping from unsigned to unsigned 
 */
#ifndef NUMBERMAP_H_
#define NUMBERMAP_H_
#ifdef SGI_HASH_MAP
#include "ExtHashMap.h"
typedef Sgi::hash_map<unsigned, unsigned> NumberMap;
#else //SGI_HASH_MAP
#include <map>
typedef std::map<unsigned, unsigned> NumberMap;
#endif //SGI_HASH_MAP
#endif //NUMBERMAP_H_

