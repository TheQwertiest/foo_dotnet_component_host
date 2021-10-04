using System;

namespace Qwr.ComponentInterface
{

    [Flags]
    public enum PreferencesPageState : UInt32
    {
        HasNoChanges = 0,
        HasChanged = 1,
        IsResettable = 8,
        NeedsFb2kRestart = 2,
        NeedsLibraryRescan = 32,
        NeedsPlaybackRestart = 4,
    }
    public struct PreferencesPageInfo
    {
        public Guid Guid;
        public Guid ParentGuid;
        public string Name;
        public string? HelpUrl;
    }
    public interface IPreferencesPageCallback
    {
        void OnStateChanged();
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