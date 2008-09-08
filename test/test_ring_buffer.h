#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include "../ring_buffer.h"
#include "minunit.h"

int tests_run = 0;
char minunit_msg[255];
char msg[50];
#define assert_start_end(buffer, s,e)  \
	{ unsigned char sa = buffer.idx_start; \
	unsigned char ea = buffer.idx_end; \
	sprintf(msg, "expected %d/%d, got %d/%d",s,e,sa,ea); mu_assert(msg, sa == s && ea == e); }

static char* test_push_pop() {
	ring_buffer buffer;
	buffer.idx_start = 0;
	buffer.idx_end = 0;

	ring_buffer_push(buffer, 1);
	ring_buffer_push(buffer, 2);
	ring_buffer_push(buffer, 3);
	assert_start_end(buffer, 0,3);

	mu_assert_eq(1, ring_buffer_pop(buffer));
	mu_assert_eq(2, ring_buffer_pop(buffer));
	mu_assert_eq(3, ring_buffer_pop(buffer));
	assert_start_end(buffer, 3,3);

	ring_buffer_push(buffer, 4);
	ring_buffer_push(buffer, 5);
	ring_buffer_push(buffer, 6);
	ring_buffer_push(buffer, 7);
	assert_start_end(buffer, 3,7);

	mu_assert_eq(4, ring_buffer_pop(buffer));
	mu_assert_eq(5, ring_buffer_pop(buffer));
	mu_assert_eq(6, ring_buffer_pop(buffer));
	mu_assert_eq(7, ring_buffer_pop(buffer));
	assert_start_end(buffer, 7,7);

	ring_buffer_push(buffer, 8);
	assert_start_end(buffer, 7,8);

	mu_assert_eq(8, ring_buffer_pop(buffer));
	assert_start_end(buffer, 8,8);

	ring_buffer_push(buffer, 9);
	ring_buffer_push(buffer, 10);
	ring_buffer_push(buffer, 11);
	ring_buffer_push(buffer, 12);
	assert_start_end(buffer, 8, 12);

	mu_assert_eq(9, ring_buffer_pop(buffer));
	mu_assert_eq(10, ring_buffer_pop(buffer));
	mu_assert_eq(11, ring_buffer_pop(buffer));
	mu_assert_eq(12, ring_buffer_pop(buffer));
	assert_start_end(buffer, 12, 12);

	return 0;
}

static char * test_index_overflow() {
	ring_buffer buffer;
	buffer.idx_start = 0;
	buffer.idx_end = 0;
	int i = 0;
	for (i=0; i<2000; i++) {
		assert_start_end(buffer, i%256, i%256);
		ring_buffer_push(buffer, i);
		assert_start_end(buffer, i%256, (i+1)%256);
		char msg[20];
		int popped = ring_buffer_pop(buffer);
		sprintf(msg, "popped value should be %d, is %d", i, popped);
		mu_assert(msg, (popped == i%256));
	}
	return 0;
}

static char * test_ring_buffer_push_array() {
	ring_buffer buffer;
	buffer.idx_start = 0;
	buffer.idx_end = 0;
	uint8_t array[8] = { 1,2,3,4,5,6,7,8 };
	ring_buffer_push_array(buffer, array, 4 );
	assert_start_end(buffer, 0, 4);
	mu_assert_eq(1, ring_buffer_pop(buffer));
	mu_assert_eq(2, ring_buffer_pop(buffer));
	mu_assert_eq(3, ring_buffer_pop(buffer));
	mu_assert_eq(4, ring_buffer_pop(buffer));

	ring_buffer_push_array(buffer, array, 8 );
	assert_start_end(buffer, 4, 12);

	mu_assert_eq(1, ring_buffer_pop(buffer));
	mu_assert_eq(2, ring_buffer_pop(buffer));
	mu_assert_eq(3, ring_buffer_pop(buffer));
	mu_assert_eq(4, ring_buffer_pop(buffer));
	mu_assert_eq(5, ring_buffer_pop(buffer));
	mu_assert_eq(6, ring_buffer_pop(buffer));
	mu_assert_eq(7, ring_buffer_pop(buffer));
	mu_assert_eq(8, ring_buffer_pop(buffer));

	assert_start_end(buffer, 12, 12);

	return 0;
}

char * test_ring_buffer() {
	mu_run_test(test_push_pop);
	mu_run_test(test_index_overflow);
	mu_run_test(test_ring_buffer_push_array);
	return 0;
}
