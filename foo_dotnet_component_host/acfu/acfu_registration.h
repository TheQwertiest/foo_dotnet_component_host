#pragma once

using namespace System;

namespace Qwr::DotnetHost
{

void RegisterAcfu( Guid guid, String ^ componentName, String ^ componentFilename, String ^ componentRepo, String ^ repoOwner );

} // namespace Qwr::DotnetHost
