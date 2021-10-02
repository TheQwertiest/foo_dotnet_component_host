#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost::Convert::ToNative
{

std::string ToValue( String ^ inValue );
GUID ToValue( Guid ^ inValue );
metadb_handle_ptr ToValue( IMetadbHandle ^ inValue );
metadb_handle_list ToValue( List< IMetadbHandle^> ^ inValue );

}
