#include <stdafx.h>

#include "assembly_loader.h"

using namespace System::Reflection;
using namespace System;
using namespace System::IO;

namespace
{

static Assembly ^ LoadFromSameFolder( Object ^ sender, ResolveEventArgs ^ args ) {
    auto folderPath =
        Path::GetDirectoryName( Assembly::GetExecutingAssembly()->Location );
    auto assemblyPath =
        Path::Combine( folderPath, ( gcnew AssemblyName( args->Name ) )->Name + ".dll" );
    if ( !File::Exists( assemblyPath ) )
    {
        return nullptr;
    }

    return Assembly::Load( AssemblyName::GetAssemblyName( assemblyPath ) );
}

}

namespace Qwr::DotnetHost
{

void InitializeAssemblyLoader()
{
    auto currentDomain = AppDomain::CurrentDomain;
    currentDomain->AssemblyResolve += gcnew ResolveEventHandler( ::LoadFromSameFolder );
}

ComponentLoadContext::ComponentLoadContext( String ^ pluginPath )
    : _resolver( pluginPath )
{
}

Assembly ^ ComponentLoadContext::Load( AssemblyName ^ assemblyName )
{
    auto assemblyPath = _resolver.ResolveAssemblyToPath( assemblyName );
    if ( assemblyPath == nullptr )
    {
        return nullptr;
    }

    return LoadFromAssemblyPath( assemblyPath );
}

IntPtr ComponentLoadContext::LoadUnmanagedDll( String ^ unmanagedDllName )
{
    auto libraryPath = _resolver.ResolveUnmanagedDllToPath( unmanagedDllName );
    if ( libraryPath == nullptr )
    {
        return IntPtr::Zero;
    }

    return LoadUnmanagedDllFromPath( libraryPath );
}

} // namespace Qwr::DotnetHost
