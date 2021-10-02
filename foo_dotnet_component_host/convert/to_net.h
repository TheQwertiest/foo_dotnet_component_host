#pragma once

#include <string_view>

using namespace System;

namespace Qwr::DotnetHost::Convert::ToNet
{

String ^ ToValue( const char* inValue ) = delete;
String ^ ToValue( std::string_view inValue );
String ^ ToValue( const pfc::string_base& inValue );
String ^ ToValue( const pfc::exception& inValue );
Guid  ToValue( const GUID& inValue );

} // namespace Qwr::DotnetHost::Convert::ToNet
