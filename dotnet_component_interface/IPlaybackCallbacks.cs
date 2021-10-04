using System;

namespace Qwr.ComponentInterface
{
    public enum PlaybackStartCommand
    {
        Default = 0,
        Play = 1,
        Next = 2,
        Previous = 3,
        Random = 5
    }

    public enum PlaybackStopReason
    {
        User = 0,
        EndOfFile = 1,
        StartingAnother = 2,
        ShuttingDown = 3,
    }

    public class GenericEventArgs<T> : EventArgs
    {
        public T Value { get; private set; }

        public GenericEventArgs(T Value) { this.Value = Value; }
    }

    public class PlaybackStartingEventArgs : EventArgs
    {
        public readonly PlaybackStartCommand StartCommand;
        public readonly bool WasPaused;

        public PlaybackStartingEventArgs(PlaybackStartCommand startCommand,
                                         bool wasPaused)
        {
            StartCommand = startCommand;
            WasPaused = wasPaused;
        }
    }

    public interface IPlaybackCallbacks
    {
        event EventHandler<GenericEventArgs<IMetadbHandle>> PlaybackAdvancedToNewTrack;
        event EventHandler<PlaybackStartingEventArgs> PlaybackStarting;
        event EventHandler<GenericEventArgs<PlaybackStopReason>> PlaybackStopped;
        event EventHandler<GenericEventArgs<bool>> PlaybackPausedStateChanged;
        event EventHandler DynamicPlaybackTrackInfoChanged;
        event EventHandler DynamicTrackInfoChanged;
        event EventHandler<GenericEventArgs<IMetadbHandle>> CurrentTrackInfoChanged;
        event EventHandler<GenericEventArgs<double>> TrackPlaybackPositionChanged;
        event EventHandler<GenericEventArgs<double>> TrackSeekPerformed;
        event EventHandler<GenericEventArgs<float>> VolumeChanged;
    }
}