using System;
using System.Drawing;

namespace Qwr.ComponentInterface
{
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
        MainMenuGroups_File,
        MainMenuGroups_Edit,
        MainMenuGroups_View,
        MainMenuGroups_Playback,
        MainMenuGroups_Library,
        MainMenuGroups_Help,
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
        bool IsFb2kMinimized();
        string ProfilePath();
        [RequiresInitialization]
        void ShowPopup(string text, string title);
    }
}