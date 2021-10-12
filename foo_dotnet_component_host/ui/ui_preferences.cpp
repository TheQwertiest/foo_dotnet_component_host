#include <stdafx.h>

#include "ui_preferences.h"

#include <convert/to_net.h>
#include <ui/ui_preferences_form.h>

namespace Qwr::DotnetHost
{

ComponentInterface::PreferencesPageInfo Preferences::GetInfo()
{
    PreferencesPageInfo info;
    info.Name = DNET_NAME;
    info.Guid = Convert::ToNet::ToValue( Guids::ui_preferences );
    info.ParentGuid = Convert::ToNet::ToValue( preferences_page::guid_components );
    info.HelpUrl = "https://theqwertiest.github.io/foo_dotnet_component_host/";

    return info;
}

void Preferences::Initialize( IntPtr parentHandle, IPreferencesPageCallback ^ callback )
{
    preferencesCallback_ = callback;

    _impl = gcnew PreferencesForm( this );
    SetParent( (HWND)_impl->Handle.ToPointer(), (HWND)parentHandle.ToPointer() );
    _impl->Anchor = ( AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom );
    _impl->Show();
}

void Preferences::Reset()
{
}

void Preferences::Apply()
{
    assert( _impl );

    _impl->Apply();
}

PreferencesPageState Preferences::State()
{
    assert( _impl );

    PreferencesPageState state = PreferencesPageState::HasNoChanges;
    if ( _impl->HasComponentChanges() )
    {
        state = PreferencesPageState::HasChanged | PreferencesPageState::NeedsFb2kRestart;
    }

    return state;
}

IntPtr Preferences::Handle()
{
    assert( _impl );
    return _impl->Handle;
}

IPreferencesPageCallback ^ Preferences::Callback()
{
    return preferencesCallback_;
}

} // namespace Qwr::DotnetHost
