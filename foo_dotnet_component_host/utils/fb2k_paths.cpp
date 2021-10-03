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
    std::string profile_path = core_api::get_profile_path();
    if ( profile_path._Starts_with( "file://" ) )
    {
        profile_path = profile_path.substr( sizeof( "file://" ) - 1 );
    }

    return fs::u8path( profile_path ).lexically_normal().u8string();
}

#pragma managed

} // namespace Qwr::DotnetHost
