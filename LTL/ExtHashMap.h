//Pseudo include file for SGI's hash_map and hash_multimap
#ifndef EXTHASHMAP_H_
#define EXTHASHMAP_H_
#ifdef __GNUC__
#if __GNUC__ >= 3
#include <ext/hash_map>
#if __GNUC_MINOR__ == 0
namespace Sgi = std;               // GCC 3.0
#else
namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
#endif // __GNUC_MINOR__
#else  //  __GNUC__ < 3
#include <hash_map>
namespace Sgi { using ::hash_map; using ::hash_multimap;}; 
#endif // __GNUC__ >= 3
#else      // __GNUC__ 
namespace Sgi = std;
#include HASH_MAP_LOC
#endif  //__GNUC__
#endif //EXTHASHMAP_H_
