#include <stdafx.h>
#include "fb_console.h"

#include <convert/to_native.h>

namespace Qwr::DotnetHost {

void NetFbConsole::Log( String ^ text )
{
    LogStatic( text );
}

void NetFbConsole::LogStatic( String ^ text )
{
     FB2K_console_formatter() << Convert::ToNative::ToValue(text).c_str();
}


} // namespace fooManagedWrapper