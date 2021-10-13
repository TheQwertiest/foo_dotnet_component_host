#include <stdafx.h>

#include "delayed_installation.h"

#include <convert/to_net.h>

#include <component_paths.h>

using namespace System::IO;

namespace
{

void ClearDelayedStatus( String ^ componentName )
{
    auto toRemoveFile = Path::Combine( Qwr::DotnetHost::Convert::ToNet::ToValue( Qwr::DotnetHost::TempDir_ComponentsToRemove() ), componentName );
    auto toInstallDir = Path::Combine( Qwr::DotnetHost::Convert::ToNet::ToValue( Qwr::DotnetHost::TempDir_ComponentsToInstall() ), componentName );

    if ( File::Exists( toRemoveFile ) )
    {
        File::Delete( toRemoveFile );
    }
    if ( Directory::Exists( toInstallDir ) )
    {
        Directory::Delete( toInstallDir, true );
    }
}

} // namespace

namespace Qwr::DotnetHost
{

ComponentDelayedStatus GetComponentDelayedStatus( String ^ componentName )
{
    auto toRemoveFile = Path::Combine( Convert::ToNet::ToValue( TempDir_ComponentsToRemove() ), componentName );
    auto toInstallDir = Path::Combine( Convert::ToNet::ToValue( TempDir_ComponentsToInstall() ), componentName );

    if ( File::Exists( toRemoveFile ) )
    {
        return ComponentDelayedStatus::ToBeRemoved;
    }
    else if ( Directory::Exists( toInstallDir ) )
    {
        return ComponentDelayedStatus::ToBeUpdated;
    }
    else
    {
        return ComponentDelayedStatus::NotDelayed;
    }
}

void ClearAllComponentDelayedStatuses()
{
    auto toRemoveDir = Convert::ToNet::ToValue( TempDir_ComponentsToRemove() );
    auto toInstallDir = Convert::ToNet::ToValue( TempDir_ComponentsToInstall() );

    if ( Directory::Exists( toRemoveDir ) )
    {
        Directory::Delete( toRemoveDir, true );
    }
    if ( Directory::Exists( toInstallDir ) )
    {
        Directory::Delete( toInstallDir, true );
    }
}

void MarkComponentAsToBeRemoved( String ^ componentName )
{
    ClearDelayedStatus( componentName );

    auto toRemoveFile = Path::Combine( Convert::ToNet::ToValue( TempDir_ComponentsToRemove() ), componentName );
    Directory::CreateDirectory( Path::GetDirectoryName( toRemoveFile ) );
    File::Create( toRemoveFile );
}

void MarkComponentAsToBeInstalled( String ^ componentName, String ^ componentContentDir )
{
    ClearDelayedStatus( componentName );

    auto toInstallDir = Path::Combine( Convert::ToNet::ToValue( TempDir_ComponentsToInstall() ), componentName );
    Directory::CreateDirectory( Path::GetDirectoryName( toInstallDir ) );
    Directory::Move( componentContentDir, toInstallDir );
}

void ProcessDelayedComponents()
{
    try
    {
        auto toRemoveDir = Convert::ToNet::ToValue( TempDir_ComponentsToRemove() );
        auto toInstallDir = Convert::ToNet::ToValue( TempDir_ComponentsToInstall() );

        if ( Directory::Exists( toRemoveDir ) )
        {
            for each ( auto f in Directory::EnumerateFiles( toRemoveDir ) )
            {
                auto componentDir = Path::Combine( Convert::ToNet::ToValue( NetComponentsDir() ), Path::GetFileNameWithoutExtension( f ) );
                Directory::Delete( componentDir, true );
            }
        }
        if ( Directory::Exists( toInstallDir ) )
        {
            for each ( auto f in Directory::EnumerateDirectories( toInstallDir ) )
            {
                auto componentDir = Path::Combine( Convert::ToNet::ToValue( NetComponentsDir() ), Path::GetFileNameWithoutExtension( f ) );
                if ( Directory::Exists( componentDir ) )
                {
                    Directory::Delete( componentDir, true );
                }
                Directory::CreateDirectory( Path::GetDirectoryName( componentDir ) );
                Directory::Move( f, componentDir );
            }
        }

        ClearAllComponentDelayedStatuses();
    }
    catch ( Exception ^ /*e*/ )
    {
        ClearAllComponentDelayedStatuses();
        throw;
    }
}

} // namespace Qwr::DotnetHost
