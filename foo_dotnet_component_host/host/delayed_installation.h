#pragma once

using namespace System;

namespace Qwr::DotnetHost
{

enum class ComponentDelayedStatus
{
    ToBeRemoved,
    ToBeUpdated,
    NotDelayed
};

ComponentDelayedStatus GetComponentDelayedStatus( String ^ componentName );
void ClearAllComponentDelayedStatuses();

void MarkComponentAsToBeRemoved( String ^ componentName );
void MarkComponentAsToBeInstalled( String ^ componentName, String ^ componentContentDir );

void ProcessDelayedComponents();

} // namespace Qwr::DotnetHost
