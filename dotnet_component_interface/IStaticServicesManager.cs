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

        /// <summary>
        /// This will only work if `foo_acfu` component is installed (but it's safe to call even if it's not)
        /// </summary>
        void RegisterAcfu(Guid guid, string componentRepo, string repoOwner);

        // TODO: implement dynamic main menu here
        // IMainMenuNode RegisterMainMenuRoot(MainMenuItemInfo info, Guid
        // parentGuid);
    }
}