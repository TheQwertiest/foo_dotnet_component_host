#pragma once

namespace Qwr::DotnetHost
{

ref class NetFbPlayCallback;

class PlayCallbackImpl : public play_callback
{
public:
    PlayCallbackImpl( NetFbPlayCallback ^ parent );

    void on_playback_new_track( metadb_handle_ptr p_track ) override;
    void on_playback_time( double p_time ) override;
    void on_playback_stop( play_control::t_stop_reason reason ) override;
    void on_playback_pause( bool p_state ) override;
    void on_playback_dynamic_info_track( const file_info& p_info ) override;
    void on_playback_starting( play_control::t_track_command p_command, bool p_paused ) override;
    void on_playback_seek( double p_time ) override;
    void on_playback_edited( metadb_handle_ptr p_track ) override;
    void on_playback_dynamic_info( const file_info& p_info ) override;
    void on_volume_change( float p_new_val ) override;

private:
    gcroot<NetFbPlayCallback ^> parent_;
};

} // namespace Qwr::DotnetHost
