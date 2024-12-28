#ifdef USE_HEADER_UNITS
module;
#else
#pragma once

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
#endif


#ifdef USE_HEADER_UNITS
export module stl;



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

// export namespace std {

// 	namespace filesystem {
//     using std::filesystem::absolute;
//     using std::filesystem::canonical;
//     using std::filesystem::copy;
//     using std::filesystem::copy_file;
//     using std::filesystem::copy_options;
//     using std::filesystem::create_directories;
//     using std::filesystem::directory_iterator;
//     using std::filesystem::directory_options;
//     using std::filesystem::exists;
//     using std::filesystem::file_size;
//     using std::filesystem::filesystem_error;
//     using std::filesystem::path;
//     using std::filesystem::read_symlink;
//     using std::filesystem::remove;
//     using std::filesystem::remove_all;
//     using std::filesystem::rename;
//     using std::filesystem::resize_file;

//     using std::filesystem::is_directory;
//     using std::filesystem::is_regular_file;
//     using std::filesystem::is_symlink;
//     using std::filesystem::recursive_directory_iterator;
//     }
// }

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

// PUSH_MACRO(FLT_MIN);
// #undef FLT_MIN
// EXPORT_POP_MACRO(FLT_MIN);

// PUSH_MACRO(FLT_MAX);
// #undef FLT_MAX
// EXPORT_POP_MACRO(FLT_MAX);

// PUSH_MACRO(DBL_MIN);
// #undef DBL_MIN
// EXPORT_POP_MACRO(DBL_MIN);

// PUSH_MACRO(DBL_MAX);
// #undef DBL_MAX
// EXPORT_POP_MACRO(DBL_MAX);


#endif