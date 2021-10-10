#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class NetFbConsole sealed : public IConsole
{
public:
    virtual void Log( String ^ text );
};

} // namespace Qwr::DotnetHost
