#include <stdio.h>

#define N 8
#include "../../common/bits.h"
#include "signed_division.h"

void test_exhaustive();
void test_boundaries();

int main() {
#if N == 8 || N == 16
	printf("Testing all %u-bit signed integers. This might take a while...\n", N);
	test_exhaustive();
	printf("Done!\n");
#elif N == 32
	printf("Testing boundaries of all %u-bit signed integers divisors. This will take a while...\n", N);
	test_boundaries();
	printf("Done!\n");
#endif
	return 0;
}

// Test quotient n/d for all n, d in S_N with d != 0
void test_exhaustive() {
    for (sint d = SINT_MIN; true; d++) {
		if (d == 0) d++;
		uint d_abs = abs(d);
		sdivdata_t dd = precompute_signed(d);
        for (sint n = SINT_MIN; true; n++) {
			if (!(d == -1 && n == SINT_MIN)) {
				assert(fast_signed_divide(n, dd) == n / d);
				if (n == SINT_MAX) break;
			}
        }
        if (d == SINT_MAX) break;
    }
}

// For every divisor d in S_N with d != 0 test the quotient
// for all dividends of the form k * d or k * d - 1.
void test_boundaries() {
	for (sint d = 1; true; d++) {
		sdivdata_t dd = precompute_signed(d);
		
		assert(fast_signed_divide(0, dd) == 0);
		assert(fast_signed_divide(1, dd) == 1 / d);
		assert(fast_signed_divide(UINT_MAX, dd) == UINT_MAX / d);
		
		uint bound = UINT_MAX / d;
		for (uint k = 1, n = d; true; k++) {
			assert(fast_signed_divide(n, dd) == k);
			assert(fast_signed_divide(n - 1, dd) == k - 1);
			if (k == bound) break;
			n += d;
		}
		
		if (d == SINT_MAX) break;
	}
}
