#include <stdafx.h>

#include "main_menu.h"

#include <convert/to_native.h>
#include <net_objects/fb_main_menu.h>

namespace QwrConvert = Qwr::DotnetHost::Convert;

namespace
{

using namespace Qwr::DotnetHost;

class MainMenuCommands_Impl : public mainmenu_commands
{
public:
    MainMenuCommands_Impl( GUID parentGuid, uint32_t sortPriority );

    t_uint32 get_command_count() override;
    GUID get_command( t_uint32 p_index ) override;
    void get_name( t_uint32 p_index, pfc::string_base& p_out ) override;
    bool get_description( t_uint32 p_index, pfc::string_base& p_out ) override;
    GUID get_parent() override;
    void execute( t_uint32 p_index, service_ptr_t<service_base> p_callback ) override;
    bool get_display( t_uint32 p_index, pfc::string_base& p_out, t_uint32& p_flags ) override;

    void SetCommands( List<NetFbMainMenuCommand ^> ^ commands );

private:
    List<NetFbMainMenuCommand ^> ^ GetCommands();

private:
    const GUID parentGuid_;
    const uint32_t sortPriority_;

    gcroot<List<NetFbMainMenuCommand ^> ^> commands_;
};

class MainMenuGroup_Impl : public mainmenu_group_popup_v2
{
public:
    MainMenuGroup_Impl( GUID guid, GUID parentGuid, const std::string& name, uint32_t sortPriority, bool isPopup );

    GUID get_guid() override;
    GUID get_parent() override;
    t_uint32 get_sort_priority() override;
    void get_display_string( pfc::string_base& p_out ) override;
    bool popup_condition() override;

private:
    const GUID guid_;
    const GUID parentGuid_;
    const std::string name_;
    const uint32_t sortPriority_;
    const bool isPopup_;
};

} // namespace

namespace
{

using namespace Qwr::DotnetHost;

MainMenuCommands_Impl::MainMenuCommands_Impl( GUID parentGuid, uint32_t sortPriority )
    : parentGuid_( parentGuid )
    , sortPriority_( sortPriority )
{
}

t_uint32 MainMenuCommands_Impl::get_command_count()
{
    return commands_->Count;
}

GUID MainMenuCommands_Impl::get_command( t_uint32 p_index )
{
    if ( p_index >= static_cast<uint32_t>( commands_->Count ) )
    {
        assert( false );
        throw std::runtime_error( "Unexpected command index received" );
    }

    return QwrConvert::ToNative::ToValue( GetCommands()[p_index]->GetGuid() );
}

void MainMenuCommands_Impl::get_name( t_uint32 p_index, pfc::string_base& p_out )
{
    if ( p_index >= static_cast<uint32_t>( commands_->Count ) )
    {
        assert( false );
        throw std::runtime_error( "Unexpected command index received" );
    }

    auto name = QwrConvert::ToNative::ToValue( GetCommands()[p_index]->GetName() );
    p_out.set_string( name.c_str(), name.size() );
}

bool MainMenuCommands_Impl::get_description( t_uint32 p_index, pfc::string_base& p_out )
{
    if ( p_index >= static_cast<uint32_t>( commands_->Count ) )
    {
        assert( false );
        throw std::runtime_error( "Unexpected command index received" );
    }

    auto description = QwrConvert::ToNative::ToValue( GetCommands()[p_index]->GetDescription() );
    if ( description.empty() )
    {
        return false;
    }

    p_out.set_string( description.data(), description.size() );
    return true;
}

GUID MainMenuCommands_Impl::get_parent()
{
    return parentGuid_;
}

void MainMenuCommands_Impl::execute( t_uint32 p_index, service_ptr_t<service_base> p_callback )
{
    if ( p_index >= static_cast<uint32_t>( commands_->Count ) )
    {
        assert( false );
        throw std::runtime_error( "Unexpected command index received" );
    }

    GetCommands()[p_index]->GetAction()();
}

bool MainMenuCommands_Impl::get_display( t_uint32 p_index, pfc::string_base& p_out, t_uint32& p_flags )
{
    if ( p_index >= static_cast<uint32_t>( commands_->Count ) )
    {
        assert( false );
        throw std::runtime_error( "Unexpected command index received" );
    }

    auto command = GetCommands()[p_index];
    auto name = QwrConvert::ToNative::ToValue( command->GetName() );
    p_out.set_string( name.c_str(), name.size() );

    if ( command->IsAlwaysHidden )
    {
        return false;
    }

    p_flags = 0;
    if ( !command->IsEnabled )
    {
        p_flags |= flag_disabled;
    }
    if ( command->IsChecked )
    {
        p_flags |= flag_checked;
    }
    if ( command->IsRadioChecked )
    {
        p_flags |= flag_radiochecked;
    }
    if ( command->IsDefaultHidden )
    {
        p_flags |= flag_defaulthidden;
    }

    return true;
}

void MainMenuCommands_Impl::SetCommands( List<NetFbMainMenuCommand ^> ^ commands )
{
    assert( commands != nullptr );
    commands_ = commands;
}

List<NetFbMainMenuCommand ^> ^ MainMenuCommands_Impl::GetCommands()
{
    return commands_;
}

MainMenuGroup_Impl::MainMenuGroup_Impl( GUID guid, GUID parentGuid, const std::string& name, uint32_t sortPriority, bool isPopup )
    : guid_( guid )
    , parentGuid_( parentGuid )
    , name_( name )
    , sortPriority_( sortPriority )
    , isPopup_( isPopup )
{
}

GUID MainMenuGroup_Impl::get_guid()
{
    return guid_;
}

GUID MainMenuGroup_Impl::get_parent()
{
    return parentGuid_;
}

t_uint32 MainMenuGroup_Impl::get_sort_priority()
{
    return sortPriority_;
}

void MainMenuGroup_Impl::get_display_string( pfc::string_base& p_out )
{
    p_out.set_string( name_.c_str(), name_.size() );
}

bool MainMenuGroup_Impl::popup_condition()
{
    return isPopup_;
}

} // namespace

namespace
{

std::vector<std::unique_ptr<service_factory_single_t<MainMenuCommands_Impl>>> g_mainMenuCommands;
std::vector<std::unique_ptr<service_factory_single_t<MainMenuGroup_Impl>>> g_mainMenuGroups;

} // namespace

namespace Qwr::DotnetHost
{

void RegisterMainMenuCommandSection( GUID parentGuid, uint32_t sortPriority, List<NetFbMainMenuCommand ^> ^ commands )
{
    assert( commands != nullptr );

    auto& pElem = g_mainMenuCommands.emplace_back( std::make_unique<service_factory_single_t<MainMenuCommands_Impl>>( parentGuid, sortPriority ) );
    pElem->get_static_instance().SetCommands( commands );
}

void RegisterMainMenuGroup( GUID guid, GUID parentGuid, const std::string& name, uint32_t sortPriority, bool isPopup )
{
    g_mainMenuGroups.emplace_back( std::make_unique<service_factory_single_t<MainMenuGroup_Impl>>(
        guid, parentGuid, name, sortPriority, isPopup ) );
}

} // namespace Qwr::DotnetHost
