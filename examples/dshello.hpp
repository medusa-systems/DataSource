
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <cstdint> // For int64_t etc.

#include <list>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stack>
#include <thread>

#define kB			1024
#define MB			(1024 * 1024)
#define GB			(1024 * 1024 * 1024)

// CBPOW = Circular-buffer power, i.e. buffer size = 2^CBPOW.
#define DEFAULT_READER_CBPOW	16
#define DEFAULT_CHANNEL_CBPOW	16
#define CONTAINER_FRAMES	(32 * kB)


#define DBG_MSG(FUNC, MSG, ...)  \
	     fprintf(stderr, "[ " #FUNC " ]\t" MSG "\n", ##__VA_ARGS__)
	     
using std::byte;
using std::string;
using std::vector;
using std::list;
using std::move; 


// Byte-counted data sizes:
typedef	int64_t 	int8;
typedef	uint64_t 	uint8;
typedef int32_t		int4;
typedef uint32_t	uint4;
typedef int16_t		int2;
typedef uint16_t	uint2;
typedef unsigned char	uint1;

typedef	float	 	SAMPLE;

