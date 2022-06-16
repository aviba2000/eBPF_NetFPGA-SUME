#include "parsing_helpers_test.h"
#include "writing_helpers_test.h"
#include "function_helpers_test.h"
#include "fast_helpers_test.h"

enum Testing_files {
	all,
	parsing_helpers_test,
	writing_helpers_test,
	function_helpers_test,
	fast_helpers_test
};

enum Testing_files test_file = all;

int main() {
	int ret = 1;
	switch(test_file) {
		case all:
			ret = main_parsing();
			if (ret < 0) break;
			ret += main_writing();
			if (ret < 0) break;
			ret += main_function();
			if (ret < 0) break;
			ret += main_fast_function();
			break;
		case parsing_helpers_test:
			ret = main_parsing();
			break;
		case writing_helpers_test:
			ret = main_writing();
			break;
		case function_helpers_test:
			ret = main_function();
			break;
		case fast_helpers_test:
			ret = main_fast_function();
			break;
	}
	return ret;
}
