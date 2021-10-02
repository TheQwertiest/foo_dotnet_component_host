#include <stdafx.h>

#include "fb_preferences_page_callback.h"

namespace Qwr::DotnetHost
{

NetFbPreferencesPageCallback::NetFbPreferencesPageCallback( preferences_page_callback::ptr& callback )
    : pCallback_( &callback )
{
}

NetFbPreferencesPageCallback::~NetFbPreferencesPageCallback()
{
    this->!NetFbPreferencesPageCallback();
}

NetFbPreferencesPageCallback::!NetFbPreferencesPageCallback()
{
    pCallback_ = nullptr;
}

void NetFbPreferencesPageCallback::OnStateChanged()
{
    if ( pCallback_ )
    {
        ( *pCallback_ )->on_state_changed();
    }
}

} // namespace Qwr::DotnetHost
