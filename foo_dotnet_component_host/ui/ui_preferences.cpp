#include <stdafx.h>

#include "ui_preferences.h"

#include <convert/to_net.h>
#include <ui/ui_preferences_form.h>

namespace Qwr::DotnetHost
{

Qwr::ComponentInterface::PreferencesPageInfo Preferences::GetInfo()
{
    PreferencesPageInfo info;
    info.Name = DNET_NAME;
    info.Guid = Convert::ToNet::ToValue( Guids::ui_preferences );
    info.ParentGuid = Convert::ToNet::ToValue( preferences_page::guid_components );
    // TODO: fill properly
    info.HelpUrl = nullptr;

    return info;
}

void Preferences::Initialize( IntPtr parentHandle, IPreferencesPageCallback ^ callback )
{
    PreferencesCallback = callback;

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
}

PreferencesPageState Preferences::State()
{
    PreferencesPageState state = PreferencesPageState::HasNoChanges;
    if ( _impl->HasNewComponents() )
    {
        state = PreferencesPageState::NeedsFb2kRestart;
    }

    return state;
}

System::IntPtr Preferences::Handle()
{
    return _impl->Handle;
}

} // namespace Qwr::DotnetHost
