#include <stdafx.h>

#include "fb2k_services.h"

#include <acfu/acfu_registration.h>
#include <fb2k/preferences_pages.h>
#include <host/host.h>
#include <net_objects/fb_config_var.h>
#include <net_objects/fb_main_menu.h>
#include <net_objects/fb_play_callback.h>

namespace Qwr::DotnetHost
{

void Fb2kStaticServices::RegisterPreferencesPage( PreferencesPageInfo preferencesPageInfo, Type ^ preferencePageType )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }
    Qwr::DotnetHost::RegisterPreferencesPage( preferencesPageInfo, preferencePageType );
}

generic<typename T> IConfigVar<T> ^ Fb2kStaticServices::RegisterConfigVar( Guid cfgGuid, T defaultValue )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }
    return gcnew NetFbConfigVar<T>( cfgGuid, defaultValue );
}

IMainMenuGroup ^ Fb2kStaticServices::GetMainMenuGroup( Guid mainMenuGroupGuid )
{
    return gcnew NetFbMainMenuGroup( mainMenuGroupGuid );
}

void Fb2kStaticServices::RegisterAcfu( Guid guid, String ^ componentRepo, String ^ repoOwner )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }

    auto currentComponent = Host::GetInstance()->GetCurrentComponent();
    assert( currentComponent );

    Qwr::DotnetHost::RegisterAcfu( guid, currentComponent->info->Name, currentComponent->underscoredName, componentRepo, repoOwner );
}

IPlaybackCallbacks ^ Fb2kDynamicServices::RegisterForPlaybackCallbacks()
{
    if ( !pPlaybackCallbacks_ )
    {
        pPlaybackCallbacks_ = gcnew NetFbPlayCallback();
    }

    return pPlaybackCallbacks_;
}

} // namespace Qwr::DotnetHost
