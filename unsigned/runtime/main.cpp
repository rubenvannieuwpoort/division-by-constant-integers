#include <stdio.h>

#define N 8
#include "../../common/bits.h"
#include "unsigned_division.h"

void test_exhaustive();
void test_boundaries();

int main() {
#if N == 8 || N == 16
	printf("Testing all %u-bit unsigned integers. This might take a while...\n", N);
	test_exhaustive();
	printf("Done!\n");
#elif N == 32
	printf("Testing boundaries of all %u-bit unsigned integers divisors. This will take a while...\n", N);
	test_boundaries();
	printf("Done!\n");
#endif
	return 0;
}

// Test quotient n/d for all n, d in U_N with d > 0.
void test_exhaustive() {
    for (uint d = 1; true; d++) {
        udivdata_t dd = precompute_unsigned(d);
        for (uint n = 0; true; n++) {
            assert(fast_unsigned_divide(n, dd) == n / d);
            if (n == UINT_MAX) break;
        }
        if (d == UINT_MAX) break;
    }
}

// For every division d in U_N, test the quotient
// for all dividends of the form k * d or k * d - 1.
void test_boundaries() {
	for (uint d = 1; true; d++) {
		udivdata_t dd = precompute_unsigned(d);
		
		assert(fast_unsigned_divide(0, dd) == 0);
		assert(fast_unsigned_divide(1, dd) == 1 / d);
		assert(fast_unsigned_divide(UINT_MAX, dd) == UINT_MAX / d);
		
		uint bound = UINT_MAX / d;
		for (uint k = 1, n = d; true; k++) {
			assert(fast_unsigned_divide(n, dd) == k);
			assert(fast_unsigned_divide(n - 1, dd) == k - 1);
			if (k == bound) break;
			n += d;
		}
		
		if (d == UINT_MAX) break;
	}
}
