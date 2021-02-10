#ifndef UNSIGNED_DIVISION_H
#define UNSIGNED_DIVISION_H

#include "../../common/bits.h"

typedef struct {
	uint mul, add, shift;
} udivdata_t;

udivdata_t precompute_unsigned(uint d);
uint fast_unsigned_divide(uint n, udivdata_t dd);

// For a given n, evaluate (n * mul + add) >> (N + shift),
// where add, mul, and shift are specified in udivdata_t dd.
uint fast_unsigned_divide(uint n, udivdata_t dd) {
	big_uint full_product = ((big_uint)n) * dd.mul + dd.add;
	return (full_product >> N) >> dd.shift;
}

// For a given divisor d in U_N, compute add, mul, shift such that
// (n * mul + add) >> (N + shift) = n / d for all n in U_N.
udivdata_t precompute_unsigned(uint d) {
	udivdata_t divdata;
	uint l = floor_log2(d);
	
	if (d == (1 << l)) {
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

#endif
