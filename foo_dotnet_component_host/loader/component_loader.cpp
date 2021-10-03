#include <stdafx.h>

#include "component_loader.h"

#include <fb2k/component_registration.h>
#include <loader/assembly_loader.h>
#include <net_objects/fb_console.h>

using namespace System::Reflection;
using namespace System;
using namespace System::IO;

namespace Qwr::DotnetHost
{

ComponentLoader::ComponentLoader()
{
    InitializeAssemblyLoader();
}

List<IComponent ^> ^ ComponentLoader::LoadComponentsInDir( System::String ^ dirName, System::String ^ filePrefix )
{
    try
    {
        auto di = gcnew DirectoryInfo( dirName );
        auto files = di->GetFiles( filePrefix + "*.dll" );

        auto components = gcnew List<IComponent ^>();
        for each ( auto f in files )
        {
            try
            {
                if ( f->Name == DNET_DLL_NAME )
                {
                    continue;
                }
                auto component = LoadComponent( f->FullName );
                components->Add( component );
                // TODO: consider moving this somewhere more proper
                RegisterComponent( Path::GetFileNameWithoutExtension( f->Name ), component->GetInfo() );
            }
            catch ( Exception ^ e )
            {
                auto msg = gcnew String( "Error loading .NET component:\n" );
                msg += "\tcomponent name: " + f->FullName + "\n";
                msg += "\terror: " + e->Message;
                NetFbConsole::LogStatic( msg );
            }
        }

        return components;
    }
    catch ( Exception ^ e )
    {
        auto msg = gcnew String( "Error loading .NET component:\n" );
        msg += "\tdirectory: " + dirName + "\n";
        msg += "\terror: " + e->Message;
        NetFbConsole::LogStatic( msg );

        return gcnew List<IComponent ^>();
    }
}

IComponent ^ ComponentLoader::LoadComponent( System::String ^ assemblyName )
{
    pfc::hires_timer timer;
    timer.start();

    FB2K_console_formatter() << ( "0" + std::to_string( static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms" ).c_str();

    auto clientType = IComponent::typeid;
    auto loadContext = gcnew ComponentLoadContext( assemblyName );
    auto assembly = loadContext->LoadFromAssemblyName( AssemblyName::GetAssemblyName( assemblyName ) );

    FB2K_console_formatter() << ( "1" + std::to_string( static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms" ).c_str();

    for each ( Type ^ t in assembly->GetTypes() )
    {
        if ( clientType->IsAssignableFrom( t ) )
        {
            FB2K_console_formatter() << ( "2" + std::to_string( static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms" ).c_str();
            return CreateInstance( t );
        }
    }

    throw gcnew Exception( "Failed to bind assembly: " + assemblyName );
};

IComponent ^ ComponentLoader::CreateInstance( Type ^ type )
{
    auto ctorInfo = type->GetConstructor( Type::EmptyTypes );
    return dynamic_cast<IComponent ^>( ctorInfo->Invoke( nullptr ) );
}

} // namespace Qwr::DotnetHost
