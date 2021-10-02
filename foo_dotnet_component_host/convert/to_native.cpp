#include <stdafx.h>

#include "to_native.h"
#include <net_objects/fb_metadb_handle.h>

#include <msclr\marshal_cppstd.h>

using namespace msclr::interop;

namespace Qwr::DotnetHost::Convert::ToNative
{

std::string ToValue( String ^ inValue )
{
    if ( inValue == nullptr )
    {
        throw gcnew ArgumentNullException();
    }

    marshal_context ctx;
    const auto wstr = ctx.marshal_as<std::wstring>( inValue );

    size_t stringLen = WideCharToMultiByte( CP_UTF8,
                                            0,
                                            wstr.data(),
                                            wstr.size(),
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr );
    std::string outValue;
    outValue.resize( stringLen );

    stringLen = WideCharToMultiByte( CP_UTF8,
                                     0,
                                     wstr.data(),
                                     wstr.size(),
                                     outValue.data(),
                                     outValue.size(),
                                     nullptr,
                                     nullptr );
    outValue.resize( stringLen );

    return outValue;
}

GUID ToValue( Guid ^ inValue )
{
    if ( inValue == nullptr )
    {
        throw gcnew ArgumentNullException();
    }

    auto guidData = inValue->ToByteArray();
    pin_ptr<Byte> data = &( guidData[0] );

    return *(_GUID*)data;
}

metadb_handle_ptr ToValue( IMetadbHandle ^ inValue )
{
    if ( inValue == nullptr )
    {
        throw gcnew ArgumentNullException();
    }

    auto concreteMetadb = dynamic_cast<NetFbMetadbHandle ^>( inValue );
    if ( concreteMetadb == nullptr )
    {
        throw gcnew Exception( "Unexpected derived type of IMetadbHandle" );
    }
    return concreteMetadb->Handle();
}

metadb_handle_list ToValue( List<IMetadbHandle^> ^ inValue )
{
    if ( inValue == nullptr )
    {
        throw gcnew ArgumentNullException();
    }

    metadb_handle_list handleList;
    for each ( auto netHandle in inValue )
    {
        handleList.add_item( ToValue( netHandle ) );
    }

    return handleList;
}

} // namespace Qwr::DotnetHost::Convert::ToStd
