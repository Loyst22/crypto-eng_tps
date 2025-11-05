#include "xoshiro.h"
#include <stdbool.h>
#include "second_preim.h"

int test_cs48_dm(void) {
	const uint32_t m[4] = {0, 1, 2, 3};
	const uint64_t h = 0x010203040506;

	uint64_t result = cs48_dm(m, h);

	return (result == 0x5DFD97183F91);
}

int main(void)
{
	if (test_vector_okay()) {
		printf("Correct implementation of speck48_96\n");
	} else {
		printf("Incorrect implementation of speck48_96\n");
	}
  
	
	if (test_sp48_inv()) {
		printf("Correct implementation of speck48_96_inv\n");
	} else {
		printf("Incorrect implementation of speck48_96_inv\n");
	}
	
	// TODO modify the test test_cs48_dm here
	int res = test_cs48_dm();
	printf("%d\n", res);

	if (test_cs48_dm_fp()) {
		printf("Correct implementation of get_cs48_dm_fp\n");
	} else {
		printf("Incorrect implementation of get_cs48_dm_fp\n");
	}
	

  return 0;
}
