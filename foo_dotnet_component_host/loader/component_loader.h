#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private
ref class ComponentLoader
{
public:
    ComponentLoader();

public:
    List<IComponent ^> ^ LoadComponentsInDir( String ^ dirName, String ^ filePrefix );

private:
    IComponent ^ LoadComponent( String ^ assemblyName );
    IComponent ^ CreateInstance( Type ^ type );
};

} // namespace Qwr::DotnetLoader
