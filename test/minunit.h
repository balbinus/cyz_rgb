/* file: minunit.h (modified from: http://www.jera.com/techinfo/jtns/jtn002.html) */

#define mu_assert(message, test) \
	do { if (!(test)) { \
		sprintf(minunit_msg, "ASSERT: %s:%d - %s", __FILE__, __LINE__, message); \
		return minunit_msg; }\
	} while (0)

#define mu_assert_eq(expected, actual) {\
	int a = (expected); \
	int b = (actual); \
	do { if ((a) != (b)) { \
		sprintf(minunit_msg, "ASSERT_EQ: %s:%d expected %d, got %u", __FILE__, __LINE__, a, b);\
		return minunit_msg; }; \
	} while(0); }

#define mu_run_test(test) do { char *message = test(); tests_run++; \
	if (message) return message; } while (0)

#define mu_run_suite(suite) do { char* result = suite(); \
	if (result != 0) {printf("FAILURE %s\n", result); return 1; } } while(0)

extern int tests_run;
