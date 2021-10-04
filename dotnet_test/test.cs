using Qwr.ComponentInterface;
using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace fooTest
{
    public class Test : IComponent
    {
        IConfigVar<string> _myCfg;

        string initLog;

        public void Initialize(IStaticServicesManager servicesManager, IUtils utils)
        {
            _myCfg = servicesManager.RegisterConfigVar(
                 Guid.Parse("{627C0767-1234-44f8-8087-BE4934311282}"), "ololo");

            PreferencesPageInfo info;
            info.Name = "1";
            info.Guid = Guid.NewGuid();
            info.ParentGuid = Guid.Parse("{627C0767-0793-44f8-8087-BE4934311282}");
            info.HelpUrl = null;
            servicesManager.RegisterPreferencesPage(info, typeof(PrefPage));

            _utils = utils;

            initLog += utils.Fb2kGuid(Fb2kGuidId.PrefPage_Tagging).ToString() + " ";
            initLog += utils.Fb2kPath() + " ";
            initLog += utils.HostVersion().ToString() + " ";

            var fileGroup = servicesManager.GetMainMenuGroup(
                Guid.Parse("{8F487F1F-419F-47a7-8ECF-EC44AF4449A3}"));
            var group = fileGroup.AddGroup(Guid.NewGuid(), "test");
            var cmdSection = group.AddCommandSection();
            var cmd = cmdSection.AddCommand(Guid.NewGuid(), "cmd 0", "azaza",
                                            () => { Log("Lazor!"); });
            cmdSection.AddCommand(Guid.NewGuid(), "cmd 1", "ololo",
                                  () => { Log("Lazor2!"); });
            cmd.IsChecked = true;
            cmd.IsDefaultHidden = true;
        }

        public void Log(string msg) { _console.Log(msg); }

        public IComponent.Info GetInfo()
        {
            IComponent.Info info;
            info.Name = "Test plugin";
            info.Version = new Version("1.0.0");
            info.Description = "Just a test plugin, really";
            return info;
        }
        public void Start(IDynamicServicesManager servicesManager,
                          IControls controls)
        {
            _console = controls.Console();
            _console.Log("init");
            _console.Log(initLog);

            _console.Log(_utils.ProfilePath().ToString());
            _console.Log(_utils.Fb2kVersion().ToString());
            _console.Log(_utils.IsFb2kMinimized().ToString());

            _callbacks = servicesManager.RegisterForPlaybackCallbacks();

            _callbacks.PlaybackAdvancedToNewTrack += (sender, argWrapper) =>
            {
                // var meta = argWrapper.Value;
                //_console.Log(controls.TitleFormat("%title%").EvalWithMetadb(meta));
                controls.ExecuteMainMenuCommand("cmd 0");
            };
        }
        public void Shutdown() { _console.Log("quit"); }

        private IPlaybackCallbacks _callbacks;
        private IConsole _console;
        private IUtils _utils;
    }

    public class PrefPage : IPreferencesPage
    {
        [DllImport("user32")]
        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        public PrefPage() { }

        public void Apply() { }

        IntPtr IPreferencesPage.Handle() { return impl.Handle; }

        public void Initialize(IntPtr parentHandle,
                               IPreferencesPageCallback callback)
        {
            impl = new Form1();
            SetParent(impl.Handle, parentHandle);
            impl.Anchor = (AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right |
                           AnchorStyles.Bottom);
            impl.Show();
        }

        public void Reset() { }

        public PreferencesPageState State() { return 0; }

        private Form impl;
    }
}