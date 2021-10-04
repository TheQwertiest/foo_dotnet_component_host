namespace Qwr.ComponentInterface
{
    public interface IPlaybackControls
    {
        IMetadbHandle NowPlaying();
        double TrackPlaybackPosition();
        bool IsPlaying();
        bool IsPaused();
        void Play();
        void Pause();
        void Next();
        void Prev();
    }
}