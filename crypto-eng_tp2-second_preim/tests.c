#include "xoshiro.h"
#include <stdbool.h>
#include "second_preim.h"

int main(void)
{
	printf("================ Tests ================\n\n");

	printf("Question 1\n    ");
	if (test_vector_okay()) {
		printf("Correct implementation of speck48_96\n\n");
	} else {
		printf("Incorrect implementation of speck48_96\n\n");
	}
  
	printf("Question 2\n    ");
	if (test_sp48_inv()) {
		printf("Correct implementation of speck48_96_inv\n\n");
	} else {
		printf("Incorrect implementation of speck48_96_inv\n\n");
	}


	printf("Question 3\n    ");
	if (test_cs48_dm()) {
		printf("Correct implementation of cs48_dm\n\n");
	} else {
		printf("Incorrect implementation of cs48_dm\n\n");
	}

	// int res = test_cs48_dm();
	// printf("%d\n", res);

	printf("Question 4\n    ");
	if (test_cs48_dm_fp()) {
		printf("Correct implementation of get_cs48_dm_fp\n\n");
	} else {
		printf("Incorrect implementation of get_cs48_dm_fp\n\n");
	}
	
	printf("Others\n    ");
	if (test_conv_macro()) {
		printf("Correct implementation of macros\n\n");
	} else {
		printf("Incorrect implementation of macros\n\n");
	}

  return 0;
}
