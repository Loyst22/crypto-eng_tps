#include "xoshiro.h"
#include <stdbool.h>
#include "second_preim.h"

/* Test against EP 2013/404, App. C */
bool test_vector_okay()
{
    uint32_t k[4] = {0x1a1918, 0x121110, 0x0a0908, 0x020100};
    uint32_t p[2] = {0x6d2073, 0x696874};
    uint32_t c[2];
    speck48_96(k, p, c);
    printf("c = %X || %X\n", c[0], c[1]);

    return (c[0] == 0x735E10) && (c[1] == 0xB6445D);
}

bool test_sp48_inv()
{
    uint32_t k[4] = {0x1a1918, 0x121110, 0x0a0908, 0x020100};
    uint32_t p[2] = {0x6d2073, 0x696874};
	uint32_t p_verif[2] = {0};
    uint32_t c[2];
    speck48_96(k, p, c);
	speck48_96_inv(k, c, p_verif);
    printf("p = %X || %X\n", p_verif[0], p_verif[1]);

    return (p[0] == p_verif[0]) && (p[1] == p_verif[1]);
}

int test_cs48_dm(void) {
  const uint32_t m[4] = {0, 1, 2, 3};
  const uint64_t h = 0x010203040506ULL;

	uint64_t result = cs48_dm(m, h);

  return (result == 0x5DFD97183F91ULL);
}


bool test_cs48_dm_fp(void) 
{
	uint32_t m[4] = {0x1a1918, 0x121110, 0x0a0908, 0x020100};

	uint64_t fp = get_cs48_dm_fp(m);

	return (cs48_dm(m, fp) == fp);
}


bool test_conv_macro(void)
{
	uint32_t x[2] = {0x6d2073, 0x696874};

	uint64_t y = CONV_24_to_48(x);

	uint32_t z[2];
	z[0] = (y & MASK_24);
    z[1] = ((y & (MASK_24 << 24)) >> 24);

	return (x[0] == z[0] && x[1] == z[1]);
}

bool test_em(void) 
{
  #define N_TEST 16

  uint32_t m1[4] = {0};
  uint32_t m2[4] = {0};
  find_exp_mess(m1, m2);

  uint32_t m[4 * (1 + N_TEST)] = {0};

  m[0] = m1[0];
  m[1] = m1[1];
  m[2] = m1[2];
  m[3] = m1[3];
  memcpy(&m[4], m2, sizeof(uint32_t)*4);
  uint64_t h1 = hs48(m, 2, 0, 0); // hs48(m1||m2)

  for (uint32_t i = 1; i <= N_TEST; i++) {

    memcpy(&m[4 + (4 * i)], m2, sizeof(uint32_t)*4);
    uint64_t h_i = hs48(m, (i+2), 0, 0); // hs48(m1||m2||..||m2)

    if (h1 != h_i) {
      return false;
    }
  }

  return true;
}

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


	printf("================ Part 2 ================\n\n");

  printf("Question 1\n  ");
  if (test_em()) {
		printf("Correct implementation of find_exp_mess\n\n");
	} else {
		printf("Incorrect implementation of find_exp_mess\n\n");
	}

  return 0;
}
