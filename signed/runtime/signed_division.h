#ifndef SIGNED_DIVISION_H
#define SIGNED_DIVISION_H

#include "../../common/bits.h"

typedef struct {
	uint mul;
	uint shift;
	sint exor;
} sdivdata_t;

#define abs(x) ((x) < 0 ? (-(x)) : (x))

sdivdata_t precompute_signed(sint d);
sint fast_signed_divide(sint n, sdivdata_t dd);

// For a given n, evaluate +/- ((n * mul + add) >> (N + shift)) + (n >> (N - 1))
// where add, mul, and shift are specified in divdata_t dd.
sint fast_signed_divide(sint n, sdivdata_t dd) {
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
sdivdata_t precompute_signed(sint d) {
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

#endif
