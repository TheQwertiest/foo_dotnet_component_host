using System;

namespace Qwr.ComponentInterface
{
    public struct ComponentInfo
    {
        public string Name;
        public Version Version;
        public string Description;
    }

    public interface IComponent
    {
        ComponentInfo GetInfo();
        void Initialize(IStaticServicesManager servicesManager, IUtils utils);
        void Start(IDynamicServicesManager servicesManager, IControls controls);
        void Shutdown();
    }
}