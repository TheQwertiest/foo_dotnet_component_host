#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

ref struct Component
{
    String ^ fullPath = nullptr;
    String ^ dllName = nullptr;
    ComponentInfo ^ info = nullptr;
    IComponent ^ instance = nullptr;
};

} // namespace Qwr::DotnetHost
