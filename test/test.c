#include "test_ring_buffer.h"

int main(int argc, char **argv) {
	char *result = test_ring_buffer();
	if (result != 0) {
		printf("FAILURE: %s\n", result);
	}
	else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
