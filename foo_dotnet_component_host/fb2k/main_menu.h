#pragma once

using namespace System::Collections::Generic;

namespace Qwr::DotnetHost
{

    ref class NetFbMainMenuCommand;

void RegisterMainMenuCommandSection( GUID guid, GUID parentGuid, uint32_t sortPriority, List<NetFbMainMenuCommand ^>^ commands );
void RegisterMainMenuGroup( GUID guid, GUID parentGuid, const std::string& name, uint32_t sortPriority, bool isPopup );

} // namespace Qwr::DotnetHost
