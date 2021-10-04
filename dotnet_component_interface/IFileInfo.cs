using System;
using System.Collections.Generic;

namespace Qwr.ComponentInterface
{
    public struct FileInfoInfo
    {
        public string Name;
        public string Value;
    }

    public struct FileInfoMeta
    {
        public string Name;
        public IEnumerable<string> Values;
    }

    public interface IFileInfo : IDisposable
    {
        IEnumerable<FileInfoInfo> InfoEnum();
        IEnumerable<FileInfoMeta> MetaEnum();
    }
}