//Pseudo include file for SGI's hash_set
#ifndef EXTHASHSET_H_
#define EXTHASHSET_H_
#ifdef __GNUC__
#if __GNUC__ >= 3
#include <ext/hash_set>
#if __GNUC_MINOR__ == 0
namespace Sgi = std;               // GCC 3.0
#else
namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
#endif // __GNUC_MINOR__
#else // __GNUC__ < 3
#include <hash_set>
namespace Sgi { using ::hash_set; };
#endif // __GNUC__ > 3
#else      // __GNUC__ 
namespace Sgi = std;
#include HASH_SET_LOC
#endif //__GNUC__
#endif // EXTHASHSET_H_

