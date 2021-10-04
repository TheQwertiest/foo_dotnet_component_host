using System.Collections.Generic;

namespace Qwr.ComponentInterface
{
    public interface IControls
    {
        IConsole Console();
        void ExecuteMainMenuCommand(string command);
        void ExecuteContextMenuCommand(string command, List<IMetadbHandle>? metadbHandles = null);
        IPlaybackControls PlaybackControls();
        ITitleFormat TitleFormat(string expression);
    }
}