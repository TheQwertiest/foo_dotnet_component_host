#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class Fb2kStaticServices sealed : public IStaticServicesManager
{
public:
    virtual void RegisterPreferencesPage( PreferencesPageInfo preferencesPageInfo, Type ^ preferencePageType );

    generic<typename T> virtual IConfigVar<T> ^ RegisterConfigVar( Guid cfgGuid, T defaultValue );

    virtual IMainMenuGroup ^ GetMainMenuGroup( Guid mainMenuGroupGuid );

    virtual void RegisterAcfu( Guid guid, String ^ componentRepo, String ^ repoOwner );
};

private
ref class Fb2kDynamicServices sealed : public IDynamicServicesManager
{
public:
    virtual IPlaybackCallbacks ^ RegisterForPlaybackCallbacks();

private:
    IPlaybackCallbacks ^ pPlaybackCallbacks_ = nullptr;
};

} // namespace Qwr::DotnetHost
