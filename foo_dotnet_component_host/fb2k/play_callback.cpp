#include <stdafx.h>

#include "play_callback.h"

#include <net_objects/fb_file_info.h>
#include <net_objects/fb_metadb_handle.h>
#include <net_objects/fb_play_callback.h>

namespace Qwr::DotnetHost
{
PlayCallbackImpl::PlayCallbackImpl( NetFbPlayCallback ^ parent )
    : parent_( parent )
{
    assert( core_api::are_services_available() );

    static_api_ptr_t<play_callback_manager> pcm;
    pcm->register_callback( this,
                            flag_on_playback_all | flag_on_volume_change,
                            false );
}

void PlayCallbackImpl::on_playback_new_track( metadb_handle_ptr p_track )
{
    parent_->OnPlaybackAdvancedToNewTrack( gcnew NetFbMetadbHandle( p_track ) );
}

void PlayCallbackImpl::on_playback_time( double p_time )
{
    parent_->OnTrackPlaybackPositionChanged( p_time );
}

void PlayCallbackImpl::on_playback_pause( bool p_state )
{
    parent_->OnPlaybackPausedStateChanged( p_state );
}

void PlayCallbackImpl::on_playback_stop( play_control::t_stop_reason reason )
{
    parent_->OnPlaybackStopped( PlaybackStopReason( reason ) );
}

void PlayCallbackImpl::on_playback_dynamic_info_track( const file_info& /*p_info*/ )
{
    parent_->OnDynamicPlaybackTrackInfoChanged();
}

void PlayCallbackImpl::on_playback_starting( play_control::t_track_command p_command, bool p_paused )
{
    parent_->OnPlaybackStarting( gcnew PlaybackStartingEventArgs( PlaybackStartCommand( p_command ), p_paused ) );
}

void PlayCallbackImpl::on_playback_seek( double p_time )
{
    parent_->OnTrackSeekPerformed( p_time );
}

void PlayCallbackImpl::on_playback_edited( metadb_handle_ptr p_track )
{
    parent_->OnCurrentTrackInfoChanged( gcnew NetFbMetadbHandle( p_track ) );
}

void PlayCallbackImpl::on_playback_dynamic_info( const file_info& /*p_info*/ )
{
    parent_->OnDynamicTrackInfoChanged();
}

void PlayCallbackImpl::on_volume_change( float p_new_val )
{
    parent_->OnVolumeChanged( p_new_val );
}

} // namespace Qwr::DotnetHost
