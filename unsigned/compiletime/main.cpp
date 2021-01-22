#define N 8
#include "../../common/compiler.h"
#include <stdio.h>
#include <assert.h>

void examples();
void test_exhaustive();
void test_boundaries();

expression_t div_by_const_uint(const uint d, expression_t n);
expression_t div_fixpoint(uint d, expression_t n);

int main(void) {
	printf("N = %u\nSome examples of generated instructions:\n\n", N);
	examples();
#if N == 8 || N == 16
	printf("Testing all %u-bit unsigned integers. This might take a while...\n", N);
	test_exhaustive();
	printf("Done!\n");
#elif N == 32
	printf("Testing boundaries of all 32-bit unsigned integers divisors. This will take a while...\n");
	test_boundaries();
	printf("Done!\n");
#endif
	return 0;
}

// Show the instructions generated for sample divisors
void examples() {
	variable_t input = variable(0);
	uint inputs[] = { 1, 3, 7, 16, 14, 28 };
	
	for (uint d : inputs) {
		expression_t e = div_by_const_uint(d, input);
		printf("div%u:\n%s\tret\n\n", d, e->code().c_str());
	}
}

// Test quotient n/d for all n, d in U_N with d > 0
void test_exhaustive() {
	variable_t input = variable(0);
	for (uint d = 1; true; d++) {
		expression_t e = div_by_const_uint(d, input);
		
		for (uint n = 0; true; n++) {
			input->assign(n);
			assert(e->evaluate() == n / d);
			if (n == UINT_MAX) break;
		}
		if (d == UINT_MAX) break;
	}
}

// For every division d in U_N, test the quotient
// for all dividends of the form k * d or k * d - 1
void test_boundaries() {
	variable_t input = variable(0);
	for (uint d = 1; true; d++) {
		expression_t e = div_by_const_uint(d, input);
		
		input->assign(0);
		assert(e->evaluate() == 0);
		input->assign(1);
		assert(e->evaluate() == 1 / d);
		input->assign(UINT_MAX - 1);
		assert(e->evaluate() == (UINT_MAX - 1) / d);
		input->assign(UINT_MAX);
		assert(e->evaluate() == UINT_MAX / d);
		
		for (uint n = d, k = 1; k != UINT_MAX / d; n += d, k++) {
			input->assign(n - 1);
			assert(e->evaluate() == k - 1);
			input->assign(n);
			assert(e->evaluate() == k);
		}
		
		if (d == UINT_MAX) break;
	}
}

// Returns an expression that efficiently computes
// a division by the constant value d.
expression_t div_by_const_uint(const uint d, expression_t n) {
	if (d == 1) return n;
	if (is_power_of_two(d)) return shr(n, constant(floor_log2(d)));
	if (d > UINT_MAX / 2) return gte(n, constant(d));
	return div_fixpoint(d, n);
}

// Returns an expression that efficiently computes a division by the
// constant value d, where d is not a power of two (this is important
// because in this case we can just add one to m to round up).
expression_t div_fixpoint(uint d, expression_t n) {
	// test if d is efficient for round-up method

	uint l = floor_log2(d);
	uint m_down = (((big_uint)1) << (N + l)) / d;
	uint m_up = m_down + 1;
	uint product_mod_2N = m_up * d;
	bool use_round_up_method = product_mod_2N <= (1 << l);
	
	if (use_round_up_method) {
		while ((m_up & 1) == 0 && l > 0) {
			m_up >>= 1;
			l--;
		}

		// take the high word of the product n * m_up
		expression_t high_word = umulhi(n, constant(m_up));

		// shift right by l bits
		return shr(high_word, constant(l));
	}
	
	if ((d & 1) == 0) {
		// even divisors which are not efficient for round-up method
		// handled by doing a preshift and using round-up method
		
		// might end up with a negative pre-shift, correct this later
		int preshift = 1, postshift = l - 1;
		d >>= 1;
		while ((d & 1) == 0 && postshift > 0) {
			d >>= 1;
			preshift++;
			postshift -= 2;
			m_up = (m_up + 1) >> 1;
		}
	
		// optimize m
		while ((m_up & 1) == 0 && postshift > 0) {
			m_up >>= 1;
			postshift--;
		}
	
		// correct for over-pre-shifting
		if (postshift < 0) {
			m_up <<= 1;
			postshift++;
		}
	
		// get n_prime by right shifting n
		expression_t n_prime = shr(n, constant(preshift));

		// multiply n_prime by m_up and take the high word
		expression_t hi_word = umulhi(n_prime, constant(m_up));

		// shift the hi_word right by `postshift` bits
		return shr(hi_word, constant(postshift));
	}
	
	// round-down method
	while ((m_down & 1) == 0 && l > 0) {
		m_down >>= 1;
		l--;
	}

	// perform a saturating increment on n to get n_inc
	expression_t n_inc = sbb(add(n, constant(1)), constant(0));

	// multiply n_inc by m_down and take upper word
	expression_t hiword = umulhi(n_inc, constant(m_down));

	// shift right by l bits
	return shr(hiword, constant(l));
}
