#pragma once

#include <host/component.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class ComponentLoader
{
public:
    ComponentLoader();

public:
    List<Component ^> ^ GetComponentsInDir( String ^ dirName, String ^ filePrefix );
    void LoadComponent( Component ^ component );

private:
    IComponent ^ CreateInstance( Type ^ type );
};

} // namespace Qwr::DotnetHost
