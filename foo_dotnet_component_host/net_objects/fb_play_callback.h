#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

#define QWR_CALLBACK_VOID( name )         \
    virtual event EventHandler ^ name;    \
    void On##name()                       \
    {                                     \
        ##name( this, EventArgs::Empty ); \
    }

#define QWR_CALLBACK( name, type )           \
    virtual event EventHandler<type> ^ name; \
    void On##name( type value )              \
    {                                        \
        ##name( this, value );               \
    }

#define QWR_GENERIC_CALLBACK( name, type )                       \
    virtual event EventHandler<GenericEventArgs<type> ^> ^ name; \
    void On##name( type value )                                  \
    {                                                            \
        ##name( this, gcnew GenericEventArgs<type>( value ) );   \
    }

namespace Qwr::DotnetHost
{

class PlayCallbackImpl;

private
ref class NetFbPlayCallback sealed : public IPlaybackCallbacks
{
public:
    NetFbPlayCallback();
    ~NetFbPlayCallback();
    !NetFbPlayCallback();

public:
    QWR_GENERIC_CALLBACK( PlaybackAdvancedToNewTrack, IMetadbHandle ^ );
    QWR_CALLBACK( PlaybackStarting, PlaybackStartingEventArgs ^ );
    QWR_GENERIC_CALLBACK( PlaybackStopped, PlaybackStopReason );
    QWR_GENERIC_CALLBACK( PlaybackPausedStateChanged, bool );
    QWR_CALLBACK_VOID( DynamicPlaybackTrackInfoChanged );
    QWR_CALLBACK_VOID( DynamicTrackInfoChanged );
    QWR_GENERIC_CALLBACK( CurrentTrackInfoChanged, IMetadbHandle ^ );
    QWR_GENERIC_CALLBACK( TrackPlaybackPositionChanged, double );
    QWR_GENERIC_CALLBACK( TrackSeekPerformed, double );
    QWR_GENERIC_CALLBACK( VolumeChanged, float );

private:
    PlayCallbackImpl* pImpl_ = nullptr;
};

} // namespace Qwr::DotnetHost
