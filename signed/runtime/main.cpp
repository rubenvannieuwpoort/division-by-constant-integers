#define N 8
#include "../../common/bits.h"
#include <stdio.h>

typedef struct {
	uint mul;
	uint shift;
	sint exor;
} sdivdata_t;

#define abs(x) ((x) < 0 ? (-(x)) : (x))

sdivdata_t precompute(sint d);
sint fast_divide(sint n, sdivdata_t dd);

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
		sdivdata_t dd = precompute(d);
        for (sint n = SINT_MIN; true; n++) {
			if (!(d == -1 && n == SINT_MIN)) {
				assert(fast_divide(n, dd) == n / d);
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
		sdivdata_t dd = precompute(d);
		
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
		
		if (d == SINT_MAX) break;
	}
}

// For a given n, evaluate +/- ((n * mul + add) >> (N + shift)) + (n >> (N - 1))
// where add, mul, and shift are specified in divdata_t dd.
sint fast_divide(sint n, sdivdata_t dd) {
	big_sint full_signed_product = ((big_sint)n) * (sint)dd.mul;
	sint high_word_of_signed_product = full_signed_product >> N;
	sint high_word_of_unsigned_product = high_word_of_signed_product + n;
	sint rounded_down_quotient = high_word_of_unsigned_product >> dd.shift;
	sint quotient_rounded_toward_zero = rounded_down_quotient - (n >> (N - 1));

	// This is just inverting the sign of the quotient if dd.exor == -1.
	sint quotient_correct_sign = (quotient_rounded_toward_zero ^ dd.exor) - dd.exor;

	return quotient_correct_sign;
}

// For a given divisor d in U_N, compute add, mul, shift such that
// (n * mul + add) >> (N + shift) = n / d for all n in U_N.
sdivdata_t precompute(sint d) {
	sdivdata_t divdata;
	uint dabs = abs(d);

	// TODO: can we compute max(ceil_log2(|d|), 1) more efficiently?
	uint l = floor_log2(dabs);
	if ((1 << l) < dabs) l++;
	if (dabs == 1) l = 1;

	uint m_down = (((big_uint)1) << (N - 1 + l)) / dabs;
	uint m_up = m_down + 1;
	
	divdata.mul = m_up;
	divdata.exor = d >> (N - 1);
	
	divdata.shift = l - 1;
	return divdata;
}
