#include <csignal>
#include <exception>
#include <iostream>

#include "stacktrace.hpp"

void d() {
	int* x = 0;
	*x;
	// std::abort();
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
	std::signal(SIGABRT, zh::signal_hook);

	a();
}
