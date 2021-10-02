#include <stdafx.h>

#include "fb_title_format.h"

#include <net_objects/fb_metadb_handle.h>

#include <convert/to_native.h>
#include <convert/to_net.h>

namespace Qwr::DotnetHost
{

NetFbTitleFormat::NetFbTitleFormat( String ^ expression )
    : pTitleFormatObject_( new titleformat_object::ptr() )
{
    titleformat_compiler::get()->compile_safe( *pTitleFormatObject_, Convert::ToNative::ToValue( expression ).c_str() );
}

NetFbTitleFormat::~NetFbTitleFormat()
{
    this->!NetFbTitleFormat();
}

NetFbTitleFormat::!NetFbTitleFormat()
{
    if ( pTitleFormatObject_ )
    {
        delete pTitleFormatObject_;
        pTitleFormatObject_ = nullptr;
    }
}

String ^ NetFbTitleFormat::Eval( bool force )
{
    assert( pTitleFormatObject_ );

    auto pc = playback_control::get();
    metadb_handle_ptr handle;

    if ( !pc->is_playing() && force )
    { // Trying to get handle to any known playable location
        if ( !metadb::g_get_random_handle( handle ) )
        { // Fake handle, workaround recommended by foobar2000 devs
            metadb::get()->handle_create( handle, playable_location_impl{} );
        }
    }

    pfc::string8_fast text;
    pc->playback_format_title_ex( handle, nullptr, text, *pTitleFormatObject_, nullptr, playback_control::display_level_all );
    return Convert::ToNet::ToValue(text);
}

String ^ NetFbTitleFormat::EvalWithMetadb( IMetadbHandle ^ metadbHandle )
{
    assert( pTitleFormatObject_ );

    auto concreteMetadb = Convert::ToNative::ToValue( metadbHandle );

    pfc::string8_fast text;
    concreteMetadb->format_title( nullptr, text, *pTitleFormatObject_, nullptr );
    return Convert::ToNet::ToValue( text );
}

} // namespace Qwr::DotnetHost
