#pragma once

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::Loader;

namespace Qwr::DotnetHost
{

void InitializeAssemblyLoader();

private
ref class ComponentLoadContext : public AssemblyLoadContext
{
public:
    ComponentLoadContext( String ^ pluginPath );

protected:
    Assembly ^ Load( AssemblyName ^ assemblyName ) override;
    IntPtr LoadUnmanagedDll( String ^ unmanagedDllName ) override;

private:
    AssemblyDependencyResolver _resolver;
};

} // namespace Qwr::DotnetLoader
