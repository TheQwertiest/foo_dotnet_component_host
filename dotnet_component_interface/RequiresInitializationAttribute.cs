using System;

namespace Qwr.ComponentInterface
{
    /// <summary>
    /// If a method or a class has this attribute, then it means
    /// that they should can only be called and used after the component is
    /// inititalized
    /// </summary>
    public class RequiresInitializationAttribute : Attribute { }
}