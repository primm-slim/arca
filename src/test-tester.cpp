#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "tester.h"

void sample_test() {
  START_TEST("Sample Test");
  TEST_VAL(1,1);
  TEST_VAL(1,0);
  END_TEST;
}

int main() {
	sample_test();
}
