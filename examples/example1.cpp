#include <exception>
#include <iostream>

#include "stacktrace.hpp"

void d() {
	throw std::runtime_error("hello world");
}

void c() {
	d();
}

void b() {
	c();
}

void a() {
	b();
}

int main() {
	std::set_terminate([] {
		std::exception_ptr eptr = std::current_exception();
		if (eptr) {
			try {
				std::rethrow_exception(eptr);
			} catch (const std::exception& e) {
				std::cerr << "Unhandled exception: " << e.what() << "\n";
			} catch (...) {
				std::cerr << "Unknown unhandled exception\n";
			}
		} else {
			std::cerr << "Terminate called without an exception\n";
		}

		auto trace = zh::stacktrace();
		zh::drop_frames_until_throw(trace);
		zh::drop_frames_after_main(trace);
		for (auto& frame : trace) {
			std::cerr << frame.file << ":" << frame.line << " in " << frame.symbol << "\n";
		}

		std::cerr << "Done.\n";
		std::exit(1);
	});

	a();
}
