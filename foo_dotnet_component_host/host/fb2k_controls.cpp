#include <stdafx.h>

#include "fb2k_controls.h"

#include <convert/to_native.h>
#include <net_objects/fb_console.h>
#include <net_objects/fb_play_control.h>
#include <net_objects/fb_title_format.h>
#include <utils/menu_helpers.h>

namespace Qwr::DotnetHost
{

Fb2kControls::Fb2kControls()
    : pConsole_( gcnew Qwr::DotnetHost::NetFbConsole() )
    , pPlaybackControls_( gcnew Qwr::DotnetHost::NetFbPlayControl() )
{
}

IConsole ^ Fb2kControls::Console()
{
    return pConsole_;
}

void Fb2kControls::ExecuteContextMenuCommand( String ^ command, List<IMetadbHandle ^> ^ metadbHandles )
{
    metadb_handle_list handleList;
    if ( metadbHandles )
    {
        handleList = Convert::ToNative::ToValue( metadbHandles );
    }

    Qwr::DotnetHost::ExecuteContextMenuCommand( Convert::ToNative::ToValue( command ), handleList, contextmenu_manager::flag_view_full );
}

void Fb2kControls::ExecuteMainMenuCommand( String ^ command )
{
    Qwr::DotnetHost::ExecuteMainMenuCommand( Convert::ToNative::ToValue( command ) );
}

IPlaybackControls ^ Fb2kControls::PlaybackControls()
{
    return pPlaybackControls_;
}

ITitleFormat ^ Fb2kControls::TitleFormat( String ^ expression )
{
    return gcnew NetFbTitleFormat( expression );
}

} // namespace Qwr::DotnetHost
