#pragma once
#include <algorithm>
#include <vector>
#include "stackframe.hpp"

namespace zh {

/**
 * @brief Drop stack frames until and including the frame identified by the symbol.
 * Use this to exclude your library function that provides the stacktrace.
 */
bool drop_frames_until(std::vector<stackframe>& trace, std::string_view symbol) {
	auto frame = std::find_if(trace.begin(), trace.end(), [&symbol](auto& frame) { return frame.symbol == symbol; });

	if (frame != trace.end()) {
		trace.erase(trace.begin(), frame + 1);
		return true;
	}

	return false;
}

/**
 * @brief Drop stack frames after the frame identified by the symbol.
 * Use this to exclude unnecessary stack frames such as internal frames before main().
 */
bool drop_frames_after(std::vector<stackframe>& trace, std::string_view symbol) {
	auto frame = std::find_if(trace.rbegin(), trace.rend(), [&symbol](auto& frame) { return frame.symbol == symbol; });

	if (frame != trace.rend()) {
		trace.erase(frame.base(), trace.end());
		return true;
	}

	return false;
}

/**
 * @brief Drop stack frames until the frame where an exception was thrown.
 * This can be used to exclude internal frames after an exception was thrown and before it was printed in a crash
 * handler.
 */
bool drop_frames_until_throw(std::vector<stackframe>& trace) {
	std::string_view throw_symbol =
#ifdef _WIN32
		"CxxThrowException";
#else
		"__cxa_throw";
#endif

	return drop_frames_until(trace, throw_symbol);
}

/// Drop stack frames after main(), i.e. internal system function calls before program start.
bool drop_frames_after_main(std::vector<stackframe>& trace) {
	return drop_frames_after(trace, "main");
}

}  // namespace zh
