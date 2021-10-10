#include <stdafx.h>

#include "component_loader.h"

#include <loader/assembly_loader.h>

using namespace System::Reflection;
using namespace System;
using namespace System::IO;

namespace Qwr::DotnetHost
{

ComponentLoader::ComponentLoader()
{
    InitializeAssemblyLoader();
}

List<Component ^> ^ ComponentLoader::GetComponentsInDir( System::String ^ dirName, System::String ^ filePrefix )
{
    auto di = gcnew DirectoryInfo( dirName );
    auto files = di->GetFiles( filePrefix + "*.dll" );

    auto components = gcnew List<Component ^>();

    for each ( auto f in files )
    {
        if ( f->Name == DNET_DLL_NAME )
        {
            continue;
        }

        auto component = gcnew Component();
        component->fullPath = f->FullName;
        component->dllName = Path::GetFileNameWithoutExtension( f->FullName );
        components->Add( component );
    }

    return components;
}

void ComponentLoader::LoadComponent( Component ^ component )
{
    auto fullPath = component->fullPath;
    assert( fullPath );

    auto clientType = IComponent::typeid;
    auto loadContext = gcnew ComponentLoadContext( fullPath );
    auto assembly = loadContext->LoadFromAssemblyName( AssemblyName::GetAssemblyName( fullPath ) );

    for each ( Type ^ t in assembly->GetTypes() )
    {
        if ( clientType->IsAssignableFrom( t ) )
        {
            component->instance = CreateInstance( t );
            return;
        }
    }

    throw gcnew Exception( "Failed to find component entry point: " + fullPath );
};

IComponent ^ ComponentLoader::CreateInstance( Type ^ type )
{
    auto ctorInfo = type->GetConstructor( Type::EmptyTypes );
    if ( ctorInfo == nullptr )
    {
        throw gcnew Exception( "Can't access component constructor" );
    }

    auto ctorAttributes = ctorInfo->GetCustomAttributes( false );
    auto interfaceAttributeType = ComponentInterfaceVersionAttribute::typeid;
    auto currentVersion = Assembly::GetAssembly( interfaceAttributeType )->GetName()->Version;
    for each ( auto attr in ctorAttributes )
    {
        if ( attr->GetType() == interfaceAttributeType )
        {
            auto interfaceAttribute = dynamic_cast<ComponentInterfaceVersionAttribute ^>( attr );
            auto supportedVersion = interfaceAttribute->Version;
            if ( currentVersion->Major != supportedVersion->Major
                 || currentVersion->Minor < supportedVersion->Minor )
            {
                throw gcnew Exception( "Interface version mismatch:\n"
                                       + "  Interface version requested by component: " + supportedVersion->ToString() + "\n"
                                       + "  Host interface version: " + currentVersion->ToString() + "\n"
                                       + "Update " DNET_UNDERSCORE_NAME " to use this .NET component" );
            }
            break;
        }
    }

    return dynamic_cast<IComponent ^>( ctorInfo->Invoke( nullptr ) );
}

} // namespace Qwr::DotnetHost
