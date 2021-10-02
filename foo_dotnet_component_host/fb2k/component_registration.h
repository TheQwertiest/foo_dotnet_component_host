#pragma once

using namespace System;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

void RegisterComponent( String ^ filename, IComponent::Info ^ componentInfo );

} // namespace Qwr::DotnetHost
