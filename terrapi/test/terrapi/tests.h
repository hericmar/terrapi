#include "fmt/format.h"

#define TEST_ASSERT(cond) if (!(cond)) { throw std::runtime_error(fmt::format("{}:{}, assertion ({}) failed, at {}.", __FUNCTION__, __LINE__, #cond, __FILE__)); }

void test_config();

void test_rules();
