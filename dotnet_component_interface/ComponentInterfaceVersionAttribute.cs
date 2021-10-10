using System;

namespace Qwr.ComponentInterface
{
    [AttributeUsage(AttributeTargets.Constructor)]
    public class ComponentInterfaceVersionAttribute : Attribute
    {
        public readonly Version Version;
        public ComponentInterfaceVersionAttribute(string version)
        {
            this.Version = new Version(version);
        }
    }
}