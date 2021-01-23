#define N 8
#include "../../common/compiler.h"
#include <stdio.h>
#include <assert.h>

void test_exhaustive();
void test_boundaries();

expression_t div_by_const_sint(const sint d, expression_t n);
expression_t div_fixpoint(sint d, expression_t n);
expression_t div_by_const_signed_power_of_two(expression_t n, sint d);

int main(void) {
#if N == 8 || N == 16
	printf("Testing all %u-bit signed integers. This might take a while...\n", N);
	test_exhaustive();
	printf("Done!\n");
#elif N == 32
	printf("Testing boundaries of all 32-bit signed integers divisors. This will take a while...\n");
	test_boundaries();
	printf("Done!\n");
#endif
	return 0;
}

// TODO: examples. This is harder than for the unsigned case since
// these sequences get complicated enough to actually need register
// allocation... Not sure if I will implement this anytime soon

// Test quotient n/d for all n, d in S_N with d != 0
void test_exhaustive() {
	variable_t input = variable(0);
	for (sint d = SINT_MIN; true; d++) {
		if (d == 0) d++;
		expression_t e = div_by_const_sint(d, input);
			
		for (sint n = SINT_MIN; true; n++) {
			if (!(d == -1 && n == SINT_MIN)) {
				input->assign(n);
				assert((sint)e->evaluate() == n / d);
				if (n == SINT_MAX) break;
			}
		}
		
		if (d == SINT_MAX) break;
	}
}

// For every division d in U_N, test the quotient
// for all dividends of the form k * d or k * d - 1
void test_boundaries() {
	variable_t input = variable(0);
	for (sint d = SINT_MIN; true; d++) {
		if (d == 0) d++;
		expression_t e = div_by_const_sint(d, input);
		
		input->assign(0);
		assert(e->evaluate() == 0);

		input->assign(1);
		assert(e->evaluate() == 1 / d);
		
		input->assign(-1);
		assert(e->evaluate() == -1 / d);
		
		input->assign(SINT_MAX);
		assert(e->evaluate() == SINT_MAX / d);
		
		input->assign(SINT_MAX - 1);
		assert(e->evaluate() == (SINT_MAX - 1) / d);
		
		if (d != -1) {
			input->assign(SINT_MIN);
			assert(e->evaluate() == SINT_MIN / d);
		}

		input->assign(SINT_MIN + 1);
		assert(e->evaluate() == (SINT_MIN + 1) / d);
		
		sint k_start = (d == -1) ? SINT_MAX : SINT_MIN / d;

		for (sint k = k_start, n = k_start * d; k != SINT_MAX / d; k++, n += d) {
			input->assign(n - 1);
			assert(e->evaluate() == k - 1);
			input->assign(n);
			assert(e->evaluate() == k);
		}
				
		if (d == SINT_MAX) break;
	}
}

// Returns an expression that efficiently computes
// a division by the constant value d.
expression_t div_by_const_sint(const sint d, expression_t n) {
	if (d == 1) return n;
	if (d == -1) return neg(n);
	uint d_abs = abs(d);
	if (is_power_of_two(d_abs)) return div_by_const_signed_power_of_two(n, d);
	//if (d > UINT_MAX / 2) return gte(n, constant(d));
	return div_fixpoint(d, n);
}

expression_t div_by_const_signed_power_of_two(expression_t n, sint d) {
	uint d_abs = abs(d);
	int l = floor_log2(d_abs);

	// this equals ((2 << l) - 1) when n is negative and 0 otherwise
	expression_t addme = shr(sar(n, constant(l - 1)), constant(N - l));
	expression_t result = sar(add(n, addme), constant(l));

	if (d < 0) result = neg(result);

	return result;
}

// Returns an expression that efficiently computes a division by the
// constant value d, where d is not a power of two (this is important
// because in this case we can just add one to m to round up).
expression_t div_fixpoint(sint d, expression_t n) {
	uint d_abs = abs(d);

	uint l = floor_log2(d_abs) + 1;
	uint m_down = (((big_uint)1) << (N - 1 + l)) / d_abs;
	uint m_up = m_down + 1;
	
	// reduce m
	while ((m_up & 1) == 0 && l > 0) {
		m_up >>= 1;
		l--;
	}

	// take the high word of the product n * m_up
	expression_t high_word = imulhi(n, constant(m_up));

	if (m_up & MSB) high_word = add(high_word, n);

	expression_t quotient_round_down = sar(high_word, constant(l - 1));

	// correct when n is negative
	expression_t quotient_round_to_zero;
	if (d > 0) quotient_round_to_zero = sub(quotient_round_down, sar(n, constant(N - 1)));
	else quotient_round_to_zero = sub(sar(n, constant(N - 1)), quotient_round_down);

	return quotient_round_to_zero;
}
