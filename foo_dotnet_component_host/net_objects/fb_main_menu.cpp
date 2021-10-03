#include <stdafx.h>

#include "fb_main_menu.h"

#include <convert/to_native.h>
#include <fb2k/main_menu.h>
#include <host/host.h>

namespace Qwr::DotnetHost
{

NetFbMainMenuCommand::NetFbMainMenuCommand( Guid guid, String ^ name, String ^ description, Action ^ action )
    : guid_( guid )
    , name_( name )
    , description_( description )
    , action_( action )
{
}

bool NetFbMainMenuCommand::IsAlwaysHidden::get()
{
    return isAlwaysHidden_;
}

void NetFbMainMenuCommand::IsAlwaysHidden::set( bool value )
{
    isAlwaysHidden_ = value;
}

bool NetFbMainMenuCommand::IsChecked::get()
{
    return isChecked_;
}

void NetFbMainMenuCommand::IsChecked::set( bool value )
{
    isChecked_ = value;
}

bool NetFbMainMenuCommand::IsDefaultHidden::get()
{
    return isDefaultHidden_;
}

void NetFbMainMenuCommand::IsDefaultHidden::set( bool value )
{
    isDefaultHidden_ = value;
}

bool NetFbMainMenuCommand::IsEnabled::get()
{
    return isEnabled_;
}

void NetFbMainMenuCommand::IsEnabled::set( bool value )
{
    isEnabled_ = value;
}

bool NetFbMainMenuCommand::IsRadioChecked::get()
{
    return isRadioChecked_;
}

void NetFbMainMenuCommand::IsRadioChecked::set( bool value )
{
    isRadioChecked_ = value;
}

Guid ^ NetFbMainMenuCommand::GetGuid()
{
    return guid_;
}

String ^ NetFbMainMenuCommand::GetName()
{
    return name_;
}

String ^ NetFbMainMenuCommand::GetDescription()
{
    return description_;
}

Action ^ NetFbMainMenuCommand::GetAction()
{
    return action_;
}

NetFbMainMenuCommandSection::NetFbMainMenuCommandSection( Guid parentGuid, Nullable<uint16_t> sortPriority )
    : childCommands_( gcnew List<NetFbMainMenuCommand ^>() )
{
    RegisterMainMenuCommandSection( Convert::ToNative::ToValue( parentGuid ), sortPriority.HasValue ? sortPriority.Value << 16 : mainmenu_commands::sort_priority_dontcare, childCommands_ );
}

IMainMenuCommand ^ NetFbMainMenuCommandSection::AddCommand( Guid guid, String ^ name, String ^ description, Action ^ action )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }

    auto command = gcnew NetFbMainMenuCommand( guid, name, description, action );
    childCommands_->Add( command );
    return command;
}

NetFbMainMenuGroup::NetFbMainMenuGroup( Guid guid )
    : guid_( guid )
    , childGroups_( gcnew List<NetFbMainMenuGroup ^>() )
    , childCommands_( gcnew List<NetFbMainMenuCommandSection ^>() )
{
}

NetFbMainMenuGroup::NetFbMainMenuGroup( Guid guid, Guid parentGuid, String ^ name, Nullable<uint16_t> sortPriority, bool isPopup )
    : guid_( guid )
    , childGroups_( gcnew List<NetFbMainMenuGroup ^>() )
    , childCommands_( gcnew List<NetFbMainMenuCommandSection ^>() )
{
    RegisterMainMenuGroup( Convert::ToNative::ToValue( guid ),
                           Convert::ToNative::ToValue( parentGuid ),
                           Convert::ToNative::ToValue( name ),
                           sortPriority.HasValue ? sortPriority.Value << 16 : mainmenu_commands::sort_priority_dontcare,
                           isPopup );
}

IMainMenuGroup ^ NetFbMainMenuGroup::AddGroup( Guid guid, String ^ name, Nullable<uint16_t> sortPriority, bool isPopup )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }

    auto command = gcnew NetFbMainMenuGroup( guid, guid_, name, sortPriority, isPopup );
    childGroups_->Add( command );
    return command;
}

IMainMenuCommandSection ^ NetFbMainMenuGroup::AddCommandSection( Nullable<uint16_t> sortPriority )
{
    if ( Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "Static services can't be initialized after the component start" );
    }

    auto command = gcnew NetFbMainMenuCommandSection( guid_, sortPriority );
    childCommands_->Add( command );
    return command;
}

} // namespace Qwr::DotnetHost
