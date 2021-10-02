#include <stdafx.h>
#include "fb_play_control.h"

#include <net_objects/fb_metadb_handle.h>

namespace Qwr::DotnetHost
{


IMetadbHandle ^ NetFbPlayControl::NowPlaying()
{
    metadb_handle_ptr metadb;
    if ( !playback_control::get()->get_now_playing( metadb ) )
    {
        return nullptr;
    }

    return gcnew NetFbMetadbHandle( metadb );
}

double NetFbPlayControl::TrackPlaybackPosition()
{
    return playback_control::get()->playback_get_position();
}

bool NetFbPlayControl::IsPlaying()
{
    return playback_control::get()->is_playing();
}

bool NetFbPlayControl::IsPaused()
{
    return playback_control::get()->is_paused();
}

void NetFbPlayControl::Play()
{
    standard_commands::main_play();
}

void NetFbPlayControl::Pause()
{
    standard_commands::main_pause();
}

void NetFbPlayControl::Next()
{
    standard_commands::main_next();
}

void NetFbPlayControl::Prev()
{
    standard_commands::main_previous();
}

} // namespace fooManagedWrapper