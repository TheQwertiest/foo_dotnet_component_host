using System;

namespace Qwr.ComponentInterface
{
    /// <summary>
    /// Methods of this interface can only be invoked inside `IComponent.Initialize` call
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




}