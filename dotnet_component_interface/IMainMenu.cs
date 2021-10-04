using System;

namespace Qwr.ComponentInterface
{
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
        IMainMenuCommandSection AddCommandSection(ushort? sortPriority = null);
    }
}