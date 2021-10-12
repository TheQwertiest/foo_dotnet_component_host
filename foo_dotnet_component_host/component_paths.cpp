#include <stdafx.h>

#include "component_paths.h"

namespace fs = std::filesystem;

namespace Qwr::DotnetHost
{

#pragma unmanaged

std::string ComponentDir()
{
    static const auto dir = [] {
        pfc::string8_fast tmp;
        uGetModuleFileName( core_api::get_my_instance(), tmp );

        return fs::u8path( tmp.c_str() ).parent_path().lexically_normal().u8string();
    }();

    return dir;
}

std::string Fb2kDir()
{
    static const auto dir = [] {
        pfc::string8_fast tmp;
        uGetModuleFileName( nullptr, tmp );

        return fs::u8path( tmp.c_str() ).parent_path().lexically_normal().u8string();
    }();

    return dir;
}

std::string ProfileDir()
{
    static const auto dir = [] {
        if ( core_api::are_services_available() )
        {
            std::string profile_path = core_api::get_profile_path();
            if ( profile_path._Starts_with( "file://" ) )
            {
                profile_path = profile_path.substr( sizeof( "file://" ) - 1 );
            }

            return fs::u8path( profile_path ).lexically_normal().u8string();
        }
        else
        {
            // can't use fb2k method to retrieve profile dir here, because it's not working yet.
            // hence we have to divine it: profile/user-components/current_module_dir/current_module.dll
            return fs::u8path( ComponentDir() ).parent_path().parent_path().lexically_normal().u8string();
        }
    }();

    return dir;
}

std::string ComponentWorkingDir()
{
    static const auto dir = ( fs::u8path( ProfileDir() ) / DNET_UNDERSCORE_NAME ).u8string();
    return dir;
}

std::string NetComponentsDir()
{
    static const auto dir = ( fs::u8path( ComponentWorkingDir() ) / "components" ).u8string();
    return dir;
}

std::string TempDir()
{
    static const auto dir = ( fs::u8path( ComponentWorkingDir() ) / "tmp" ).u8string();
    return dir;
}

std::string TempDir_ComponentsToInstall()
{
    static const auto dir = ( fs::u8path( TempDir() ) / "components_to_install" ).u8string();
    return dir;
}

std::string TempDir_ComponentsToRemove()
{
    static const auto dir = ( fs::u8path( TempDir() ) / "components_to_remove" ).u8string();
    return dir;
}

std::string TempDir_ComponentUnpack()
{
    static const auto dir = ( fs::u8path( TempDir() ) / "unpacked_component" ).u8string();
    return dir;
}

#pragma managed

} // namespace Qwr::DotnetHost
