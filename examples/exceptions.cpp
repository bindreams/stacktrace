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
	std::set_terminate(zh::terminate_hook);

	a();
}
