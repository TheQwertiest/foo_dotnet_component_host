#pragma once

using namespace System;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost {

private ref class NetFbConsole sealed : public IConsole
{
public:
    virtual void Log( String ^ text );
    static void LogStatic( String ^ text );
};

}
