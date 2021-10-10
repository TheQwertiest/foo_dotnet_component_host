#include <stdafx.h>

#include "delayed_log.h"

#include <convert/to_native.h>

#pragma unmanaged

namespace
{

bool g_isDrained = false;
std::vector<std::string> g_messages;

} // namespace

namespace
{

void Log( const std::string& msg, bool addComponentName, bool addNewLine )
{
    const auto formattedMsg = ( addComponentName ? DNET_UNDERSCORE_NAME ":\n" : "" ) + msg + ( addNewLine ? "\n" : "" );
    if ( !g_isDrained )
    {
        g_messages.emplace_back( formattedMsg );
    }
    else
    {
        FB2K_console_formatter() << formattedMsg.c_str();
    }
}

void Drain()
{
    for ( const auto& message: g_messages )
    {
        FB2K_console_formatter() << message.c_str();
    }
    g_messages.clear();
    g_isDrained = true;
}

} // namespace

#pragma managed

namespace Qwr::DotnetHost
{

void DelayedLog( String ^ text, bool addComponentName, bool addNewLine )
{
    Log( Convert::ToNative::ToValue( text ), addComponentName, addNewLine );
}

void DrainDelayedLog()
{
    Drain();
}

} // namespace Qwr::DotnetHost
