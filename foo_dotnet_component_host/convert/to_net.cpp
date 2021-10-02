#include <stdafx.h>

#include "to_net.h"

namespace Qwr::DotnetHost::Convert::ToNet
{

String ^ ToValue( std::string_view inValue ) {
    return gcnew String( inValue.data(), 0, inValue.size(), gcnew System::Text::UTF8Encoding( true, true ) );
}

    String
    ^ ToValue( const pfc::string_base& inValue ) {
          return ToValue( std::string_view{ inValue.c_str(), inValue.length() } );
      }

    System::String
    ^ ToValue( const pfc::exception& inValue ) {
          return ToValue( std::string_view{ inValue.what() } );
      }

    System::Guid
    ToValue( const GUID& inValue ) {
          return Guid( inValue.Data1,
                             inValue.Data2,
                             inValue.Data3,
                             inValue.Data4[0],
                             inValue.Data4[1],
                             inValue.Data4[2],
                             inValue.Data4[3],
                             inValue.Data4[4],
                             inValue.Data4[5],
                             inValue.Data4[6],
                             inValue.Data4[7] );
      }

}
