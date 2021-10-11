#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

ref struct Component
{
    String ^ fullPath = nullptr;
    String ^ underscoredName = nullptr;
    ComponentInfo ^ info = nullptr;
    Qwr::ComponentInterface::IComponent ^ instance = nullptr;
};

} // namespace Qwr::DotnetHost
