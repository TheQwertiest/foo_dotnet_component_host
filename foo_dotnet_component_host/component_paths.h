#pragma once

using namespace System;

namespace Qwr::DotnetHost
{

#pragma unmanaged

std::string ComponentDir();
std::string Fb2kDir();
std::string ProfileDir();
std::string ComponentWorkingDir();
std::string NetComponentsDir();
std::string TempDir();
std::string TempDir_ComponentsToInstall();
std::string TempDir_ComponentsToRemove();
std::string TempDir_ComponentUnpack();

#pragma managed

} // namespace Qwr::DotnetHost
