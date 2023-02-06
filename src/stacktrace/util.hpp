#pragma once
#include <string>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#if defined(__GXX_RTTI) or defined(_CPPRTTI)
#define STACKTRACE_HAS_RTTI 1
#endif

namespace zh {
namespace detail {

#ifndef _MSC_VER
std::string demangle(const char* mangled_name) {
	size_t length;
	int status;

	auto deleter = [](char* str) { std::free(str); };
	std::unique_ptr<char, decltype(deleter)> name(
		abi::__cxa_demangle(mangled_name, nullptr, &length, &status), deleter
	);

	if (name == nullptr) {
		switch (status) {
			case -1:
				std::cerr << "stacktrace: could not demangle symbol " << mangled_name
						  << ": memory allocation failure\n";
				break;
			case -2:
				break;
			case -3:
				std::cerr << "stacktrace: could not demangle symbol " << mangled_name << ": invalid argument\n";
				break;
			default:
				std::cerr << "stacktrace: could not demangle symbol " << mangled_name
						  << ": __cxa_demangle failed with code " << status << "\n";
				break;
		}

		return std::string{mangled_name};
	}

	return std::string{name.get()};
}
#endif  // _MSC_VER

#ifdef STACKTRACE_HAS_RTTI

template<typename T>
std::string typename_string(T&& val) {
#ifdef _MSC_VER
	return typeid(std::forward<T>(val)).name();
#else
	return detail::demangle(typeid(std::forward<T>(val)).name());
#endif  // _MSC_VER
}

#endif  // STACKTRACE_HAS_RTTI

}  // namespace detail
}  // namespace zh
