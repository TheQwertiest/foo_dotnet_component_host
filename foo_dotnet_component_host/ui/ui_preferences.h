#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

ref class PreferencesForm;

public
ref class Preferences : IPreferencesPage
{
public:
    static PreferencesPageInfo GetInfo();

    virtual void Initialize( IntPtr parentHandle, IPreferencesPageCallback ^ callback );
    virtual void Reset();
    virtual void Apply();
    virtual PreferencesPageState State();
    virtual IntPtr Handle();

    IPreferencesPageCallback ^ Callback();

private:
    PreferencesForm ^ _impl;
    IPreferencesPageCallback ^ preferencesCallback_;
};

} // namespace Qwr::DotnetHost
