#pragma once
#include <string>

namespace zh {

struct stackframe {
	std::string file;
	int line = 0;
	std::string symbol;
};

}  // namespace zh
