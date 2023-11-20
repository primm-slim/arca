
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <inttypes.h>

class GlobalTester {
public:
	uint32_t pass;
	uint32_t fail;
	const char * test_name;

	void start_test(const char * name) {
		test_name = name;
		pass = fail = 0;
		printf("Starting Test %s\n",name);
	}

	void end_test() {
		printf("Test Summary %s ",test_name);
		if (fail > 0) 
			printf("FAIL ");
		else if (pass > 0)
			printf("PASS ");
		else printf("NONE ");
		printf("%d passed %d failed\n",pass, fail);
	}

	void test_val(uint64_t x, uint64_t y, const char *filename, int lineno) {
		if (x == y) {
			pass += 1;
		} else {
			fail += 1;
			printf("FAILED %" PRIu64 " != %" PRIu64 "\n",x,y);
			printf("Filename: %s Line no: %d\n", filename, lineno);
		}
	}

	void watch_val(const char * label, uint64_t val, const char * filename,  int lineno) {
			printf("WATCH %s %" PRIu64  "\n",label, val);
			printf("Filename: %s Line no: %d\n", filename, lineno);
	}

	void print_val(uint64_t x) {
		printf("%" PRIu64, x);
	}
	void print_str(const char * s) {
		printf("%s", s);
	}
	void print_ln() {
		printf("\n");
	}
	void print_hex(uint64_t x) {
		printf("0x%lx", x);
	}
	void trace(const char *filename, int lineno) {
			printf("TRACE Filename: %s Line no: %d\n", filename, lineno);
	}


} global_test;



#define START_TEST(test_name) global_test.start_test(test_name)
#define END_TEST global_test.end_test()

#define TEST_VAL(x, y) global_test.test_val(x, y, __FILE__, __LINE__)

#define WATCH(s, x) global_test.watch_val(s, x, __FILE__, __LINE__)

#define PRINT_LN global_test.print_ln()
#define PRINT_STR(s) global_test.print_str(s)
#define PRINT_VAL(x) global_test.print_val(x)
#define PRINT_HEX(x) global_test.print_hex(x)
#define TRACE global_test.trace(__FILE__, __LINE__)



