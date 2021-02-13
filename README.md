# Division by constant integers

This is a proof of concept to show division by constant integers can be optimized.

For use in production, you should probably use [libdivide](libdivide.com), which implements this technique and a lot more, and is actively maintained and being used and benchmarked. This implementation is meant to give an example implementation for the articles [division by constant unsigned integers](https://rubenvannieuwpoort.nl/posts/division-by-constant-unsigned-integers) and [division by constant signed integers](https://rubenvannieuwpoort.nl/posts/division-by-constant-signed-integers) and is meant to illustrate the technique.

## Usage

Define `N` with the number of bits that you want to use, and include `bits.h`:

```
#define N 32
#include "common/bits.h"
```

Now, you can use the `uint` and `sint` datatypes, which have `N` bits.

To optimize unsigned division, you can include `unsigned/runtime/unsigned_division.h`. You can use the following functions to 
```
udivdata_t precompute_unsigned(uint d);
uint fast_unsigned_divide(uint n, udivdata_t dd);
```

Before we can perform a division, we need to do some precomputation:
```
uint divisor = get_divisor();
udivdata_t divisor_data = precompute_unsigned(divisor);
```

Now, we can perform a division:
```
uint quotient = fast_unsigned_division(n, divisor_data);
```

For signed division, use the analogous functions and datatypes (replace 'unsigned' by 'signed', 'uint' by 'sint', and 'sdiv' by 'udiv').


## Example

Let's implement a simple benchmark for 32-bit integers. First, include some headers and implement a method to count the number of nanoseconds that has passed:
```
#include <inttypes.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define N 32
#include "common/bits.h"
#include "signed/runtime/signed_division.h"
#include "unsigned/runtime/unsigned_division.h"

uint64_t get_nanoseconds() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}
```

Now we define some parameters for the benchmark:
```
const uint unsigned_divisor = 123456;
const uint32_t number_of_divisions = 1000000;
```

Now, the benchmark consists of dividing all the numbers from 1 to number_of_divisions by the divisor, and accumulate the quotients:
```
uint64_t reference_sum = 0, reference_start = get_nanoseconds();
for (uint32_t n = 1; n <= number_of_divisions; n++) {
	reference_sum += n / unsigned_divisor;
}
uint64_t reference_end = get_nanoseconds();
printf("Standard division took %.3f nanoseconds per division\n", 1.0 * (reference_end - reference_start) / number_of_divisions);

uint64_t fast_sum = 0, fast_start = get_nanoseconds();
udivdata_t d = precompute_unsigned(signed_divisor);
for (uint32_t n = 1; n <= number_of_divisions; n++) {
	fast_sum += fast_unsigned_divide(n, d);
}
uint64_t fast_end = get_nanoseconds();
printf("Fast division took %.3f nanoseconds per division\n", 1.0 * (fast_end - fast_start) / number_of_divisions);

assert(fast_sum == reference_sum);
```

For signed division, we can do the same, but we sum from `-number_of_divisions` to `number_of_divisions`:
```
const sint signed_divisor = 123456;
const uint32_t number_of_divisions = 1000000;

int64_t reference_sum = 0, reference_start = get_nanoseconds();
for (int32_t n = -number_of_divisions / 2; n <= number_of_divisions / 2; n++)
	if (n != 0)
		reference_sum += n / signed_divisor;
uint64_t reference_end = get_nanoseconds();
printf("Standard division took %.3f nanoseconds per division\n", 1.0 * (reference_end - reference_start) / number_of_divisions);

int64_t fast_sum = 0, fast_start = get_nanoseconds();
sdivdata_t d = precompute_signed(signed_divisor);
for (int32_t n = -number_of_divisions / 2; n <= number_of_divisions / 2; n++)
	if (n != 0)
		fast_sum += fast_signed_divide(n, d);
uint64_t fast_end = get_nanoseconds();
printf("Fast division took %.3f nanoseconds per division\n", 1.0 * (fast_end - fast_start) / number_of_divisions);

assert(fast_sum == reference_sum);
```

