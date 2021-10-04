#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class NetFbPreferencesPageCallback sealed : public IPreferencesPageCallback
{
public:
    NetFbPreferencesPageCallback( preferences_page_callback::ptr& callback );
    ~NetFbPreferencesPageCallback();
    !NetFbPreferencesPageCallback();

    virtual void OnStateChanged();

private:
    preferences_page_callback::ptr* pCallback_;
};

} // namespace Qwr::DotnetHost
