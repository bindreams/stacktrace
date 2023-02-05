#pragma once
#include <ranges>
#include <utility>
#include <vector>

// clang-format off
#include "Windows.h"
#include "DbgHelp.h"
// clang-format on

#include "filter.hpp"
#include "stackframe.hpp"

namespace zh {
namespace detail {

/**
 * @brief Get the initial frame for walking the stack, along with the machine type.
 *
 * @return std::pair<machine-type, initial-frame>
 */
std::pair<DWORD, STACKFRAME64> get_initial_frame(const CONTEXT& context) {
	std::pair<DWORD, STACKFRAME64> result;
	DWORD& machine_type = result.first;
	STACKFRAME64& frame = result.second;

#ifdef _M_IX86  // x86
	machine_type = IMAGE_FILE_MACHINE_I386;
	frame.AddrPC.Offset = context.Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Esp;
	frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64    // x86-64
	machine_type = IMAGE_FILE_MACHINE_AMD64;
	frame.AddrPC.Offset = context.Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Rsp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64   // Intel Itanium
	machine_type = IMAGE_FILE_MACHINE_IA64;
	frame.AddrPC.Offset = context.StIIP;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.IntSp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrBStore.Offset = context.RsBSP;
	frame.AddrBStore.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.IntSp;
	frame.AddrStack.Mode = AddrModeFlat;
#elif _M_ARM64  // ARM64
	machine_type = IMAGE_FILE_MACHINE_ARM64;
	frame.AddrPC.Offset = context.Pc;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Fp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Sp;
	frame.AddrStack.Mode = AddrModeFlat;
#else
	static_assert(false, "stacktrace: could not determine machine type");
#endif

	return result;
}

}  // namespace detail

std::vector<stackframe> stacktrace() {
	HANDLE this_process = GetCurrentProcess();
	HANDLE this_thread = GetCurrentThread();

	CONTEXT context{};
	RtlCaptureContext(&context);
	auto [machine_type, frame] = detail::get_initial_frame(context);

	SymInitialize(this_process, nullptr, true);

	std::vector<stackframe> result;
	while (true) {
		bool ok = StackWalk64(
			machine_type,
			this_process,
			this_thread,
			&frame,
			&context,
			nullptr,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			nullptr
		);
		if (!ok) break;

		stackframe sf;

		// IMAGEHLP_SYMBOL64 has a flexible array member at the end
		struct {
			IMAGEHLP_SYMBOL64 data = {};
			char _[1024] = {};
		} symbol_holder{};
		auto& symbol = symbol_holder.data;

		symbol.SizeOfStruct = sizeof(symbol_holder);
		symbol.MaxNameLength = 1024;
		DWORD64 displacement;
		if (SymGetSymFromAddr64(this_process, frame.AddrPC.Offset, &displacement, &symbol)) {
			sf.symbol = symbol.Name;
		}

		IMAGEHLP_LINE64 line = {sizeof(IMAGEHLP_LINE64)};
		DWORD offset_from_symbol = 0;
		if (SymGetLineFromAddr64(this_process, frame.AddrPC.Offset, &offset_from_symbol, &line)) {
			sf.file = line.FileName;
			sf.line = line.LineNumber;
		}

		result.push_back(std::move(sf));
	}

	// Filter stackframes inside this function
	drop_frames_until(result, "zh::stacktrace");

	return result;
}

}  // namespace zh
