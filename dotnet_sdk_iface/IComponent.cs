using System;
using System.Collections.Generic;
using System.Drawing;

namespace Qwr.ComponentBase
{
    /// <summary>
    /// If a method or a class has this attribute, then it means
    /// that they should can only be called and used after the component is
    /// inititalized
    /// </summary>
    public class RequiresInitializationAttribute : Attribute { }

    public class GenericEventArgs<T> : EventArgs
    {
        public T Value { get; private set; }

        public GenericEventArgs(T Value) { this.Value = Value; }
    }

    public interface IComponent
    {
        struct Info
        {
            public string Name;
            public Version Version;
            public string Description;
        }

        Info GetInfo();

        void Initialize(IStaticServicesManager servicesManager, IUtils utils);
        void Start(IDynamicServicesManager servicesManager, IControls controls);
        void Shutdown();
    }

    public enum Fb2kGuidId
    {
        PrefPage_Tagging,
        PrefPage_Root,
        PrefPage_Tools,
        PrefPage_Display,
        PrefPage_Playback,
        PrefPage_Visualizations,
        PrefPage_Input,
        PrefPage_Core,
        PrefPage_TagWriting,
        PrefPage_MediaLibrary,
        PrefPage_Output,
        PrefPage_Advanced,
        PrefPage_Components,
    }

    public interface IUtils
    {
        Version HostVersion();
        Guid Fb2kGuid(Fb2kGuidId id);
        [RequiresInitialization]
        Icon Fb2kIcon();
        string Fb2kPath();
        [RequiresInitialization]
        string Fb2kVersion();
        [RequiresInitialization]
        string ProfilePath();
        [RequiresInitialization]
        bool IsFb2kMinimized();
    }

    public interface IConfigVar<T>
    {
        void Set(T value);
        T Get();
    }

    public interface IMainMenuCommand
    {
        bool IsEnabled { get; set; }
        bool IsDefaultHidden { get; set; }
        bool IsAlwaysHidden { get; set; }
        bool IsChecked { get; set; }
        bool IsRadioChecked { get; set; }
    }

    public interface IMainMenuCommandSection
    {
        IMainMenuCommand AddCommand(Guid guid, string name, string description,
                                    Action action);
    }

    public interface IMainMenuGroup
    {
        IMainMenuGroup AddGroup(Guid guid, string name, ushort? sortPriority = null,
                                bool isPopup = true);
        IMainMenuCommandSection AddCommandSection(Guid guid,
                                                  ushort? sortPriority = null);
    }

    /// <summary>
    /// Methods of this interface can only be invoked inside `Initialize` call
    /// </summary>
    public interface IStaticServicesManager
    {
        IMainMenuGroup GetMainMenuGroup(Guid mainMenuGroupGuid);
        void RegisterPreferencesPage(PreferencesPageInfo preferencesPageInfo,
                                     Type preferencePageType);
        IConfigVar<T> RegisterConfigVar<T>(Guid cfgGuid, T defaultValue);

        // TODO: implement dynamic main menu here
        // IMainMenuNode RegisterMainMenuRoot(MainMenuItemInfo info, Guid
        // parentGuid);
    }

    public interface IDynamicServicesManager
    {
        IPlaybackCallbacks RegisterForPlaybackCallbacks();
    }

    public interface IFileInfo : IDisposable
    {
        struct Info
        {
            public string Name;
            public string Value;
        }

        struct Meta
        {
            public string Name;
            public IEnumerable<string> Values;
        }

        IEnumerable<Info> InfoEnum();
        IEnumerable<Meta> MetaEnum();
    }

    public enum ArtId
    {
        CoverFront = 0,
        CoverBack = 1,
        Disc = 2,
        Icon = 3,
        Artist = 4
    }

    public interface IMetadbHandle : IDisposable
    {
        Bitmap Artwork(ArtId artId);
        Bitmap ArtworkStub(ArtId artId);
        double Length();
        string Path();
        IFileInfo FileInfo();
    }

    public enum PlaybackStartCommand
    {
        Default = 0,
        Play = 1,
        Next = 2,
        Previous = 3,
        Random = 5
    }
    ;
    public enum StopReason
    {
        User = 0,
        EndOfFile = 1,
        StartingAnother = 2,
        ShuttingDown = 3,
    }
    ;
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
        event EventHandler<GenericEventArgs<IMetadbHandle>>
            PlaybackAdvancedToNewTrack;
        event EventHandler<PlaybackStartingEventArgs> PlaybackStarting;
        event EventHandler<GenericEventArgs<StopReason>> PlaybackStopped;
        event EventHandler<GenericEventArgs<bool>> PlaybackPausedStateChanged;
        event EventHandler DynamicPlaybackTrackInfoChanged;
        event EventHandler DynamicTrackInfoChanged;
        event EventHandler<GenericEventArgs<IMetadbHandle>> CurrentTrackInfoChanged;
        event EventHandler<GenericEventArgs<double>> TrackPlaybackPositionChanged;
        event EventHandler<GenericEventArgs<double>> TrackSeekPerformed;
        event EventHandler<GenericEventArgs<float>> VolumeChanged;
    }

    public interface IConsole
    {
        void Log(string text);
    }

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
    public interface ITitleFormat : IDisposable
    {
        string Eval(bool force = false);
        string EvalWithMetadb(IMetadbHandle metadbHandle);
    }

    public interface IControls
    {
        IConsole Console();
        void ExecuteMainMenuCommand(string command);
        void ExecuteContextMenuCommand(string command, List<IMetadbHandle>? metadbHandles);
        IPlaybackControls PlaybackControls();
        ITitleFormat TitleFormat(string expression);
    }

    public interface IPreferencesPageCallback
    {
        void OnStateChanged();
    }

    public struct PreferencesPageInfo
    {
        public Guid Guid;
        public Guid ParentGuid;
        public string Name;
        public string HelpUrl;
    }

    [Flags]
    public enum PreferencesPageState : UInt32
    {
        HasChanged = 1,
        IsResettable = 8,
        NeedsFb2kRestart = 2,
        NeedsLibraryRescan = 32,
        NeedsPlaybackRestart = 4,
    }

    public interface IPreferencesPage
    {
        void Initialize(IntPtr parentHandle, IPreferencesPageCallback callback);

        PreferencesPageState State();
        IntPtr Handle();
        void Apply();
        void Reset();
    }
}