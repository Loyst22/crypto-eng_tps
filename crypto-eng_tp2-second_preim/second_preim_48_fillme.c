#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>

#include "xoshiro.h"
#include "second_preim.h"
#include "hasht.h"

#define ROTL24_16(x) ((((x) << 16) ^ ((x) >> 8)) & 0xFFFFFF)
#define ROTL24_3(x) ((((x) << 3) ^ ((x) >> 21)) & 0xFFFFFF)

#define ROTL24_8(x) ((((x) << 8) ^ ((x) >> 16)) & 0xFFFFFF)
#define ROTL24_21(x) ((((x) << 21) ^ ((x) >> 3)) & 0xFFFFFF)

#define IV 0x010203040506ULL 

#define N_THREADS 12

/*
 * the 96-bit key is stored in four 24-bit chunks in the low bits of k[0]...k[3]
 * the 48-bit plaintext is stored in two 24-bit chunks in the low bits of p[0], p[1]
 * the 48-bit ciphertext is written similarly in c
 */
void speck48_96(const uint32_t k[4], const uint32_t p[2], uint32_t c[2])
{
	uint32_t rk[23];
	uint32_t ell[3] = {k[2], k[1], k[0]};

	rk[0] = k[3];

	c[0] = p[0];
	c[1] = p[1];

	/* full key schedule */
	/*
	--------------------------- key expansion --------------------------
		for i = 0..T-2
			l[i+m-1] ← (k[i] + S−α l[i]) ⊕ i
			k[i+1] ← Sβ k[i] ⊕ l[i+m-1]
		end for
	*/
	for (unsigned i = 0; i < 22; i++)
	{
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) & 0xFFFFFF; // addition (+) is done mod 2**24
		rk[i+1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	/* full encryption */
	/*
	---------------------------- encryption ----------------------------
	for i = 0..T-1
		x ← (S−α x + y) ⊕ k[i]
		y ← Sβy ⊕ x
	end for
	*/
	for (unsigned i = 0; i < 23; i++)
	{
		c[0] = ((ROTL24_16(c[0]) + c[1]) ^ rk[i]) & 0xFFFFFF; // addition (+) is done mod 2**24
		c[1] = ROTL24_3(c[1]) ^ c[0];
	}
	// end of the loop, ciphertext is in c[0] and c[1]

	return;
}



/* the inverse cipher */
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2])
{
	/* FILL ME */
	uint32_t rk[23];
	uint32_t ell[3] = {k[2], k[1], k[0]};

	rk[0] = k[3];

	p[0] = c[0];
	p[1] = c[1];

	/* full key schedule */
	/*
	--------------------------- key expansion --------------------------
		for i = 0..T-2
			l[i+m-1] ← (k[i] + S−α l[i]) ⊕ i
			k[i+1] ← Sβ k[i] ⊕ l[i+m-1]
		end for
	*/
	for (unsigned i = 0; i < 22; i++)
	{
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) & 0xFFFFFF; // addition (+) is done mod 2**24
		rk[i+1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	/* full decyption */
	/*
	---------------------------- decryption ----------------------------
	for i = T-1..0
		y ← S-β(y ⊕ x)
		x ← Sα((x ⊕ k[i]) - y)
	end for
	*/
	for (unsigned i = 23; i > 0; i--)
	{
		p[1] = ROTL24_21(p[1] ^ p[0]);
		p[0] = ROTL24_8(((p[0] ^ rk[i-1]) - p[1]) & 0xFFFFFF); // addition (+) is done mod 2**24
	}
	// end of the loop, plaintext is in p[0] and p[1]

	return;
}

/* The Davies-Meyer compression function based on speck48_96,
 * using an XOR feedforward
 * The input/output chaining value is given on the 48 low bits of a single 64-bit word,
 * whose 24 lower bits are set to the low half of the "plaintext"/"ciphertext" (p[0]/c[0])
 * and whose 24 higher bits are set to the high half (p[1]/c[1])
 */
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h)
{
    uint32_t p[2];
    p[0] = (h & MASK_24);
    p[1] = ((h & (MASK_24 << 24)) >> 24);

    uint32_t c[2] = {0};

    speck48_96(m, p, c);

    uint64_t c_combined = CONV_24_to_48(c); 
    uint64_t output = c_combined ^ h;

    return output;
}

/* Assumes message length is fourlen * four blocks of 24 bits, each stored as the low bits of 32-bit words
 * fourlen is stored on 48 bits (as the 48 low bits of a 64-bit word)
 * when padding is included, simply adds one block (96 bits) of padding with fourlen and zeros on higher bits
 * (The fourlen * four blocks must be considered “full”, otherwise trivial collisions are possible)
 */
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose)
{
	uint64_t h = IV;
	const uint32_t *mp = m;

	for (uint64_t i = 0; i < fourlen; i++)
	{
		h = cs48_dm(mp, h);
		if (verbose)
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", i, mp[0], mp[1], mp[2], mp[3], h);
		mp += 4;
	}
	if (padding)
	{
		uint32_t pad[4];
		pad[0] = fourlen & 0xFFFFFF;
		pad[1] = (fourlen >> 24) & 0xFFFFFF;
		pad[2] = 0;
		pad[3] = 0;
		h = cs48_dm(pad, h);
		if (verbose)
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", fourlen, pad[0], pad[1], pad[2], pad[3], h);
	}

	return h;
}

/* Computes the unique fixed-point for cs48_dm for the message m */
uint64_t get_cs48_dm_fp(uint32_t m[4])
{
	/* FILL ME */
	uint32_t fp[2] = {0};
    uint32_t c[2] = {0};

	speck48_96_inv(m, c, fp);

	return CONV_24_to_48(fp);
}

/* Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4])
{
	// printf("Starting MitM\n");

	entry *hashtable_h = malloc(sizeof(entry) * N);
	memset(hashtable_h, 0, sizeof(entry) * N);
	uint64_t h;
	uint64_t fp;

	// compute N possible chaining values for N random first-block messages m1
	for (uint32_t i = 0; i < N; i++) {
		// get a random m1 value
		m1[0] = xoshiro256plus_random();
		m1[2] = xoshiro256plus_random();

		// TODO change this into a hashtable insert
		h = cs48_dm(m1, IV);
		insert(hashtable_h, N, h, m1);
	}

	// printf("First part of MitM done\n");

	bool collision = false;
	uint32_t* m_collision;
	while (!collision) {
		// get a random m2 value
		m2[0] = xoshiro256plus_random();
		m2[2] = xoshiro256plus_random();

		fp = get_cs48_dm_fp(m2);

		// hashtable lookup
		m_collision = lookup(hashtable_h, N, fp);
		if (m_collision != NULL) {
			if ((cs48_dm(m_collision, IV) & MASK_48) == (fp & MASK_48)) {
				for (uint16_t i = 0; i < 4; i++) {
					m1[i] = m_collision[i];
				}
			}
			free(hashtable_h);
			return;
		}
	}
	free(hashtable_h);
	return;
}

  // For running on a thread
typedef struct thread_args {
  uint64_t fixpoint;
  uint32_t random_m[4];
  uint32_t i_block;
  entry_hash *hashtable;
  atomic_bool *collision;
} thread_args;

void find_collision_block(void *arg)
{
  thread_args *args = (thread_args*)arg;

  while (!atomic_load(args->collision))
  {
    args->random_m[0] = (uint32_t)xoshiro256plus_random();
    args->random_m[1] = (uint32_t)xoshiro256plus_random();
    args->random_m[2] = (uint32_t)xoshiro256plus_random();
    args->random_m[3] = (uint32_t)xoshiro256plus_random();
    
    // compute hash of random blocks after fp until we find a hash that exists in the hashtable
    uint64_t hash = cs48_dm(args->random_m, args->fixpoint);
    args->i_block = lookup_hash(args->hashtable, hash);

    if (args->i_block != 0) 
    {
      printf("found block\n");
      atomic_store(args->collision, true);
      return;
    }
  }

  return;
}

bool attack(void)
{
  /*
    second preimage of 2^18 blocks.
    hash = 0x7CA651E182DBULL

    1. compute expandable message with associated fixed-point fp
    2. search for a collision block cm
      so that cs48_dm(cm,fp) == one of the chaining values of mess
    3. form second preim message m2 by expanding the expandable message to an appropriate number of blocks 
      and suffixing the colliding block and then the remaining blocks identical to the ones of mess
  */	

  entry_hash *hashtable_h = malloc(sizeof(entry_hash) * (N_BLOCKS + 1));
  uint32_t *message = malloc(sizeof(uint32_t) * (4 * (N_BLOCKS)));
  uint64_t prev_hash = IV;

    // create hashtable
  for (size_t i = 0; i < (1 << 20); i+=4)
  {
    message[i + 0] = i;
    message[i + 1] = 0;
    message[i + 2] = 0;
    message[i + 3] = 0;

    // uint64_t intermediate_hash = hs48(message, (i / 4), false, false);
    uint64_t intermediate_hash = cs48_dm(&message[i], prev_hash);
    prev_hash = intermediate_hash;
    insert_hash(hashtable_h, intermediate_hash, (i / 4));

    // if (i % (1 << 16) == 0) {
    // 	printf("hashtable at 0x%x / 0x%x\n", i, (1 << 20));
    // }
  }
  printf("\tbuilt hashtable\n");
  // now we have all the intermediate hashes and can look those up fast


  uint32_t m1[4] = {0};
  uint32_t m2[4] = {0}; // chainable block
  find_exp_mess(m1, m2);
  uint64_t fp = get_cs48_dm_fp(m2);

  uint32_t random_m[4];
  uint32_t i_block = 0;

  #ifdef N_THREADS
  // ---------- MULTITHREAD ----------
  // memory for random blocks
  thread_args args[N_THREADS];
  pthread_t threads[N_THREADS];
  atomic_bool collision = false;

  for (size_t i = 0; i < N_THREADS; i++) {
    args[i].fixpoint = fp;
    args[i].collision = &collision;
    args[i].hashtable = hashtable_h;
    args[i].i_block = 0;
    
    pthread_create(&threads[i], NULL, (void *(*)(void *))find_collision_block, (void*)&args[i]);
  }
  // pthread_join(threads[0], NULL);
  for (size_t i = 0; i < N_THREADS; i++) {
      pthread_join(threads[i], NULL);
  }
  // find which thread found the block and get back data
  for (size_t i = 0; i < N_THREADS; i++) {
    if (args[i].i_block != 0)
    {
      printf("thread %zu found block\n", i);
      random_m[0] = args[i].random_m[0];
      random_m[1] = args[i].random_m[1];
      random_m[2] = args[i].random_m[2];
      random_m[3] = args[i].random_m[3];

      i_block = args[i].i_block;
    }
  }

  #else
  // ------------ SINGLE THREAD -------------

  while (i_block == 0)
  {
    random_m[0] = (uint32_t)xoshiro256plus_random();
    random_m[1] = (uint32_t)xoshiro256plus_random();
    random_m[2] = (uint32_t)xoshiro256plus_random();
    random_m[3] = (uint32_t)xoshiro256plus_random();
    // compute hash of random blocks after fp until we find a hash that exists in the hashtable
    uint64_t hash = cs48_dm(random_m, fp);
    i_block = lookup_hash(hashtable_h, hash);
  }
  printf("\nFound block %d\n", i_block);

  #endif /* ifdef N_THREADS
   */
  

  // Build second pre-image by just replacing first
  memcpy(&message[0], m1, sizeof(uint32_t) * 4);
  memcpy(&message[4], m2, sizeof(uint32_t) * 4);
  for (size_t i = 8; i < (i_block * 4); i+=4)
  {
    memcpy(&message[i], m2, sizeof(uint32_t) * 4);
  }
  memcpy(&message[i_block*4], random_m, sizeof(uint32_t) * 4);
  
  // Test collision
  uint64_t h = hs48(message, (1 << 18), true, false); // message hashed with padding enabled
  printf("hash: %lx\n", h);
  if (h == ORIGINAL_HASH)
  {
    return true;
  } else 
  {
    return false;
  }
  
  
  free(hashtable_h);
  free(message);
}

// int main()
// {
// 	attack();

// 	return 0;
// }
