#pragma once

using namespace System;

namespace Qwr::DotnetHost
{

/// @brief This should be used in situations where console logger might not be ready yet
void DelayedLog( String ^ text, bool addComponentName = true, bool addNewLine = true );

void DrainDelayedLog();

} // namespace Qwr::DotnetHost
