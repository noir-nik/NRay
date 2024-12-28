#ifdef USE_MODULES
module;
#else
#ifdef USE_HEADER_UNITS
#undef USE_HEADER_UNITS
#endif
#endif

#ifndef USE_HEADER_UNITS

#ifndef USE_MODULES
#pragma once
#endif // USE_MODULES

#include <array>
#include <atomic>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <set>
#include <span>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <chrono>
#include <filesystem>
#include <random>
#include <string>

#include <cstdio>
#include <cstring> 
#endif // USE_HEADER_UNITS



#ifdef USE_MODULES
export module stl;

#ifdef USE_HEADER_UNITS

export import <array>;
export import <atomic>;
export import <chrono>;
export import <filesystem>;
export import <fstream>;
export import <map>;
export import <memory>; 
export import <mutex>;
export import <numeric>;
export import <random>;
export import <set>;
export import <span>;
export import <string>;
export import <string_view>;
export import <thread>;
export import <tuple>;
export import <unordered_map>;
export import <vector>;

export import <iostream>;

export import <cassert>;
export import <cmath>;
export import <csignal>;
export import <cstddef>;
export import <cstdint>;
// export import <cstdio>;
export import <cstdlib>;
export import <cstring>;

#else

// NOLINTBEGIN(misc-unused-using-decls)
export namespace std {

using std::forward;
using std::forward_as_tuple;

namespace filesystem {
    using std::filesystem::absolute;
    using std::filesystem::canonical;
    using std::filesystem::copy;
    using std::filesystem::copy_file;
    using std::filesystem::copy_options;
    using std::filesystem::create_directories;
    using std::filesystem::directory_iterator;
    using std::filesystem::directory_options;
    using std::filesystem::exists;
    using std::filesystem::file_size;
    using std::filesystem::filesystem_error;
    using std::filesystem::path;
    using std::filesystem::read_symlink;
    using std::filesystem::remove;
    using std::filesystem::remove_all;
    using std::filesystem::rename;
    using std::filesystem::resize_file;

    using std::filesystem::is_directory;
    using std::filesystem::is_regular_file;
    using std::filesystem::is_symlink;
    using std::filesystem::recursive_directory_iterator;
}

// chrono
namespace chrono {
	using std::chrono::duration_cast;
	using std::chrono::system_clock;
	using std::chrono::milliseconds;
	using std::chrono::microseconds;
	using std::chrono::nanoseconds;
	using std::chrono::seconds;
	using std::chrono::minutes;
	using std::chrono::hours;
	using std::chrono::days;
	using std::chrono::weeks;
	using std::chrono::years;
	using std::chrono::months;
	using std::chrono::high_resolution_clock;
	using std::chrono::sys_time;
	using std::chrono::sys_seconds;
	using std::chrono::sys_days;
	using std::chrono::file_clock;
	using std::chrono::file_time;
	using std::chrono::duration;
	using std::chrono::time_point;
	using std::chrono::operator+;
	using std::chrono::operator-;
	using std::chrono::operator*;
	using std::chrono::operator/;
	using std::chrono::operator%;
	using std::chrono::operator==;
	using std::chrono::operator<;
	using std::chrono::operator<=>;
	using std::chrono::operator<=;
	using std::chrono::operator>;
	using std::chrono::operator>=;
}

// memory
using std::make_unique;
using std::unique_ptr;
using std::make_shared;
using std::shared_ptr;

// string
using std::string;

// vector
using std::vector;

// span
using std::span;

} // std


// C functions

export using ::exit;

// <cstdint>
export using ::int8_t;
export using ::int16_t;
export using ::int32_t;
export using ::int64_t;

export using ::int_fast8_t;
export using ::int_fast16_t;
export using ::int_fast32_t;
export using ::int_fast64_t;

export using ::int_least8_t;
export using ::int_least16_t;
export using ::int_least32_t;
export using ::int_least64_t;

export using ::intmax_t;
export using ::intptr_t;

export using ::uint8_t;
export using ::uint16_t;
export using ::uint32_t;
export using ::uint64_t;

export using ::uint_fast8_t;
export using ::uint_fast16_t;
export using ::uint_fast32_t;
export using ::uint_fast64_t;

export using ::uint_least8_t;
export using ::uint_least16_t;
export using ::uint_least32_t;
export using ::uint_least64_t;

export using ::uintmax_t;
export using ::uintptr_t;

// NOLINTEND(misc-unused-using-decls)
#endif // USE_HEADER_UNITS

#define PRAGMA(...) _Pragma(#__VA_ARGS__)
#define PUSH_MACRO(NAME) PRAGMA(push_macro(#NAME))
#define EXPORT_POP_MACRO(NAME) export inline constexpr auto NAME = PRAGMA(pop_macro(#NAME)) NAME;

PUSH_MACRO(SHRT_MIN);
#undef SHRT_MIN
EXPORT_POP_MACRO(SHRT_MIN);

PUSH_MACRO(SHRT_MAX);
#undef SHRT_MAX
EXPORT_POP_MACRO(SHRT_MAX);

PUSH_MACRO(USHRT_MAX);
#undef USHRT_MAX
EXPORT_POP_MACRO(USHRT_MAX);

PUSH_MACRO(INT_MIN);
#undef INT_MIN
EXPORT_POP_MACRO(INT_MIN);

PUSH_MACRO(INT_MAX);
#undef INT_MAX
EXPORT_POP_MACRO(INT_MAX);

PUSH_MACRO(UINT_MAX);
#undef UINT_MAX
EXPORT_POP_MACRO(UINT_MAX);

PUSH_MACRO(LONG_MIN);
#undef LONG_MIN
EXPORT_POP_MACRO(LONG_MIN);

PUSH_MACRO(LONG_MAX);
#undef LONG_MAX
EXPORT_POP_MACRO(LONG_MAX);

PUSH_MACRO(ULONG_MAX);
#undef ULONG_MAX
EXPORT_POP_MACRO(ULONG_MAX);

PUSH_MACRO(LLONG_MAX);
#undef LLONG_MAX
EXPORT_POP_MACRO(LLONG_MAX);

PUSH_MACRO(LLONG_MIN);
#undef LLONG_MIN
EXPORT_POP_MACRO(LLONG_MIN);

PUSH_MACRO(ULLONG_MAX);
#undef ULLONG_MAX
EXPORT_POP_MACRO(ULLONG_MAX);

#endif