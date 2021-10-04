using System;

namespace Qwr.ComponentInterface
{
    public interface ITitleFormat : IDisposable
    {
        string Eval(bool force = false);
        string EvalWithMetadb(IMetadbHandle metadbHandle);
    }
}