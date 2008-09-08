#include "test_ring_buffer.h"
#include "test_cyz_cmd.h"
#include "minunit.h"

Color led_curr_color;
Color led_fade_color;
uint8_t led_fade;
uint8_t led_fadespeed;

int main(int argc, char **argv) {

	mu_run_suite(test_ring_buffer);
	mu_run_suite(test_cyz_cmd);

	printf("ALL TESTS PASSED\n");
	printf("Tests run: %d\n", tests_run);

	return 0;
}
