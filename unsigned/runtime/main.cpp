#define N 16
#include "../../common/bits.h"
#include <stdio.h>

typedef struct {
	uint mul, add, shift;
} divdata_t;

divdata_t precompute(uint d);
uint fast_divide(uint n, divdata_t dd);

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
        divdata_t dd = precompute(d);
        for (uint n = 0; true; n++) {
            assert(fast_divide(n, dd) == n / d);
            if (n == UINT_MAX) break;
        }
        if (d == UINT_MAX) break;
    }
}

// For every division d in U_N, test the quotient
// for all dividends of the form k * d or k * d - 1.
void test_boundaries() {
	for (uint d = 1; true; d++) {
		divdata_t dd = precompute(d);
		
		assert(fast_divide(0, dd) == 0);
		assert(fast_divide(1, dd) == 1 / d);
		assert(fast_divide(UINT_MAX, dd) == UINT_MAX / d);
		
		uint bound = UINT_MAX / d;
		for (uint k = 1, n = d; true; k++) {
			assert(fast_divide(n, dd) == k);
			assert(fast_divide(n - 1, dd) == k - 1);
			if (k == bound) break;
			n += d;
		}
		
		if (d == UINT_MAX) break;
	}
}

// For a given n, evaluate (n * mul + add) >> (N + shift),
// where add, mul, and shift are specified in divdata_t dd.
uint fast_divide(uint n, divdata_t dd) {
	big_uint full_product = ((big_uint)n) * dd.mul + dd.add;
	return (full_product >> N) >> dd.shift;
}

// For a given divisor d in U_N, compute add, mul, shift such that
// (n * mul + add) >> (N + shift) = n / d for all n in U_N.
divdata_t precompute(uint d) {
	divdata_t divdata;
	uint l = floor_log2(d);
	
	if (is_power_of_two(d)) {
		divdata.mul = UINT_MAX;
		divdata.add = UINT_MAX;
	}
	else {
		uint m_down = (((big_uint)1) << (N + l)) / d;
		uint m_up = m_down + 1;
		uint temp = m_up * d;
		bool use_round_up_method = temp <= (1 << l);
		
		if (use_round_up_method) {
			divdata.mul = m_up;
			divdata.add = 0;
		}
		else {
			divdata.mul = m_down;
			divdata.add = m_down;
		}
	}

	divdata.shift = l;
	return divdata;
}
