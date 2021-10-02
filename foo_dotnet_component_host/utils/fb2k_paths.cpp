#include <stdafx.h>

#include "fb2k_paths.h"

namespace fs = std::filesystem;

namespace Qwr::DotnetHost
{

#pragma unmanaged

std::string ComponentPath()
{
    pfc::string8_fast tmp;
    uGetModuleFileName( core_api::get_my_instance(), tmp );

    return fs::u8path( tmp.c_str() ).parent_path().lexically_normal().u8string();
}

std::string Fb2kPath()
{
    pfc::string8_fast tmp;
    uGetModuleFileName( nullptr, tmp );

    return fs::u8path( tmp.c_str() ).parent_path().lexically_normal().u8string();
}

std::string ProfilePath()
{
    pfc::string8_fast tmp;
    // `get_profile_path` returns strings like "file://c:\documents_and_settings\username\blah\foobar2000",
    // so we need to convert them with `g_get_display_path`
    filesystem::g_get_display_path( core_api::get_profile_path(), tmp );

    return fs::u8path( tmp.c_str() ).lexically_normal().u8string();
}

#pragma managed

} // namespace Qwr::DotnetHost
