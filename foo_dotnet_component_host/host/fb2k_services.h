using namespace System;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private
ref class Fb2kStaticServices sealed : public IStaticServicesManager
{
public:
    virtual void RegisterPreferencesPage( PreferencesPageInfo preferencesPageInfo, Type ^ preferencePageType );

    generic<typename T> virtual IConfigVar<T> ^ RegisterConfigVar( Guid cfgGuid, T defaultValue );

    virtual IMainMenuGroup ^ GetMainMenuGroup( Guid mainMenuGroupGuid );
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
