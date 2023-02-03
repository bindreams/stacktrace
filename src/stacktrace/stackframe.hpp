#pragma once
#include <string>

namespace zh {

struct stackframe {
	std::string file;
	int line;
	std::string symbol;
};

}  // namespace zh
