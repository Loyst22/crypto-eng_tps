#include "xoshiro.h"


int main(void)
{
  if (test_vector_okay()) {
		printf("Correct implementation of speck48_96\n");
	} else {
		printf("Incorrect implementation of speck48_96\n");
	}

  return 0;
}
