#ifndef BITS_H
#define BITS_H

#include <stdint.h>
#include <assert.h>

#if N == 8
#define UINT uint8_t
#define BIG_UINT uint16_t
#define SINT int8_t
#define BIG_SINT int16_t
#elif N == 16
#define UINT uint16_t
#define BIG_UINT uint32_t
#define SINT int16_t
#define BIG_SINT int32_t
#elif N == 32
#define UINT uint32_t
#define BIG_UINT uint64_t
#define SINT int32_t
#define BIG_SINT int64_t
#else
#error This bit size is not supported.
#endif

typedef UINT uint;
typedef BIG_UINT big_uint;

#undef UINT_MAX
#define UINT_MAX ((uint)-1)
#define MSB (1 << (N - 1))

typedef SINT sint;
typedef BIG_SINT big_sint;

#define SINT_MIN ((-1)<<(N - 1))
#define SINT_MAX (-1-SINT_MIN)

// Returns true if x is a power of two
bool is_power_of_two(uint x) {
	return x && !(x & (x - 1));
}

// Compute the log2 of x, rounded down to the next integer
uint floor_log2(uint x) {
	assert(x > 0);
	uint count = 0;
#if N >= 64
	if (x & 0xffffffff00000000) { x >>= 32; count += 32; }
#endif
#if N >= 32
	if (N >= 32 && x & 0x00000000ffff0000) { x >>= 16; count += 16; }
#endif
#if N >= 16
	if (N >= 16 && x & 0x000000000000ff00) { x >>= 8; count += 8; }
#endif
	if (N >= 8 && x & 0x00000000000000f0) { x >>= 4; count += 4; }
	if (x & 0x000000000000000c) { x >>= 2; count += 2; }
	if (x & 0x0000000000000002) { x >>= 1; count += 1; }
	return count;
}

#endif
