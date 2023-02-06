#pragma once
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <execinfo.h>
#include <unistd.h>

#include <elfutils/libdwfl.h>

#include "util.hpp"

namespace zh {
namespace detail {

stackframe get_frame(Dwfl* dwfl, void* ip) {
	stackframe frame;

	// Get function name.
	uintptr_t ip2 = reinterpret_cast<uintptr_t>(ip);
	Dwfl_Module* module = dwfl_addrmodule(dwfl, ip2);
	char const* symbol_raw = dwfl_module_addrname(module, ip2);
	if (symbol_raw) frame.symbol = demangle(symbol_raw);

	// Get source filename and line number.
	Dwfl_Line* source = dwfl_module_getsrc(module, ip2);
	if (source) {
		Dwarf_Addr addr;
		const char* file_raw = dwfl_lineinfo(source, &addr, &frame.line, nullptr, nullptr, nullptr);
		if (file_raw) frame.file = file_raw;
	}

	return frame;
}

}  // namespace detail

std::vector<stackframe> stacktrace() {
	std::array<void*, 256> raw_trace;
	int size = backtrace(raw_trace.data(), raw_trace.size());

	// Initialize dwfl debug session
	Dwfl_Callbacks callbacks = {};
	char* debuginfo_path = nullptr;
	callbacks.find_elf = dwfl_linux_proc_find_elf;
	callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
	callbacks.debuginfo_path = &debuginfo_path;

	auto deleter = [](Dwfl* dwfl_instance) { dwfl_end(dwfl_instance); };
	std::unique_ptr<Dwfl, decltype(deleter)> dwfl_holder(dwfl_begin(&callbacks), deleter);
	Dwfl* dwfl = dwfl_holder.get();

	if (int r = dwfl_linux_proc_report(dwfl, getpid()) != 0) {
		std::cerr << "stacktrace: could not obtain stacktrace: dwfl_linux_proc_report failed with code " << r << "\n";
		return {};
	}

	if (int r = dwfl_report_end(dwfl, nullptr, nullptr) != 0) {
		std::cerr << "stacktrace: could not obtain stacktrace: dwfl_report_end failed " << r << "\n";
		return {};
	}

	// Obtain stackframes
	std::vector<stackframe> trace;
	for (int i = 0; i < size; ++i) {
		trace.push_back(detail::get_frame(dwfl, raw_trace[i]));
	}

	drop_frames_until(trace, "zh::stacktrace()");
	return trace;
}

}  // namespace zh
