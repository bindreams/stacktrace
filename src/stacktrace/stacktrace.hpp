#pragma once
#ifdef _WIN32
#include "stacktrace_win32.hpp"
#else
#include "stacktrace_posix.hpp"
#endif
