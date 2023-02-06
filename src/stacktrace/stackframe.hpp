#pragma once
#include <iomanip>
#include <ostream>
#include <string>

namespace zh {

struct stackframe {
	std::string file;
	int line = 0;
	std::string symbol;

	friend inline std::ostream& operator<<(std::ostream& os, const stackframe& frame) {
		int indent = os.width();
		for (int i = 0; i < indent; ++i) os << ' ';

		if (frame.symbol.empty()) {
			std::cerr << "<unknown>";
		} else {
			std::cerr << frame.symbol;
		}
		if (!frame.file.empty()) std::cerr << ", in " << frame.file << ":" << frame.line;

		os.width(0);
		return os;
	}
};

std::ostream& operator<<(std::ostream& os, const std::vector<stackframe>& trace) {
	int indent = os.width();

	for (auto& frame : trace) {
		os << std::setw(indent) << frame << "\n";
	}

	os.width(0);
	return os;
}

}  // namespace zh
