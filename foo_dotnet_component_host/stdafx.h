#pragma once

// clang-format off

#define NOMINMAX

#ifdef _DEBUG
#   define QWR_TRACK_LEAKS
#   define CRTDBG_MAP_ALLOC
#   include <cstdlib>
#   include <crtdbg.h>
#else
#   include <cstdlib>
#endif

#pragma unmanaged

// foobar2000 SDK
#pragma warning( push, 0 )
#   include <foobar2000\sdk\foobar2000.h>
#   include <foobar2000\sdk\component.h>
#pragma warning( pop ) 

// fmt
#define FMT_HEADER_ONLY
#include <fmt/format.h>
/// wchar_t support
#include <fmt/xchar.h>

#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

#pragma managed

#include <vcclr.h>

#include <component_defines.h>
#include <component_guids.h>

// clang-format on
