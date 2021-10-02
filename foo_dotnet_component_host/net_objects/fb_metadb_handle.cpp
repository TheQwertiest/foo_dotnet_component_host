#include <stdafx.h>
#include "fb_metadb_handle.h"

#include <convert/to_net.h>
#include <net_objects/fb_file_info.h>

#include <array>

namespace
{

const std::array<const GUID*, 5> gKnownArtGuids = {
    &album_art_ids::cover_front,
    &album_art_ids::cover_back,
    &album_art_ids::disc,
    &album_art_ids::icon,
    &album_art_ids::artist
};
}

namespace
{

 Bitmap ^ ExtractBitmap( album_art_extractor_instance_v2::ptr pExtractor, const GUID& artTypeGuid, abort_callback& abort ) {
    auto pArtData = pExtractor->query( artTypeGuid, abort );
    if ( !pArtData.is_valid() )
    {
        return nullptr;
    }

    auto stream = gcnew System::IO::UnmanagedMemoryStream( (unsigned char*)pArtData->get_ptr(), pArtData->get_size() );
    Bitmap ^ netBitmap = gcnew Bitmap( stream );
    delete stream;
    return netBitmap;
};

}

namespace Qwr::DotnetHost
{

NetFbMetadbHandle::NetFbMetadbHandle( const metadb_handle_ptr& metadbHandle )
    : pMetadbHandle_( new metadb_handle_ptr(metadbHandle) )
{
}

NetFbMetadbHandle::~NetFbMetadbHandle()
{
    this->!NetFbMetadbHandle();
}

NetFbMetadbHandle::!NetFbMetadbHandle()
{
    if ( !core_api::are_services_available() )
    {
        return;
    }

    if ( pMetadbHandle_ )
    {
        delete pMetadbHandle_;
        pMetadbHandle_ = nullptr;
    }
}

Bitmap ^ NetFbMetadbHandle::Artwork( ArtId artId )
{
    assert( pMetadbHandle_ );

    if ( static_cast<uint32_t>(artId) >= gKnownArtGuids.size() )
    {
        throw gcnew ArgumentException( "Unknown art id: " + artId.ToString(), "artId" );
    }
    const auto& artTypeGuid = *gKnownArtGuids[static_cast<uint32_t>( artId )];

    abort_callback_dummy abort;
    auto aamv2 = album_art_manager_v2::get();

    try
    {
        auto aaeiv2 = aamv2->open( pfc::list_single_ref_t<metadb_handle_ptr>( *pMetadbHandle_ ), pfc::list_single_ref_t<GUID>( artTypeGuid ), abort );
        return ExtractBitmap( aaeiv2, artTypeGuid, abort );
    }
    catch ( const pfc::exception& )
    { // not found or aborted
    }

     return nullptr;
}

Bitmap ^ NetFbMetadbHandle::ArtworkStub( ArtId artId )
{
    assert( pMetadbHandle_ );

    if ( static_cast<uint32_t>( artId ) >= gKnownArtGuids.size() )
    {
        throw gcnew ArgumentException( "Unknown art id: " + artId.ToString(), "artId" );
    }
    const auto& artTypeGuid = *gKnownArtGuids[static_cast<uint32_t>( artId )];

    abort_callback_dummy abort;
    auto aamv2 = album_art_manager_v2::get();

    try
    {
        auto aaeiv2 = aamv2->open_stub( abort );
        return ExtractBitmap( aaeiv2, artTypeGuid, abort );
    }
    catch ( const pfc::exception& )
    { // not found or aborted
    }

    return nullptr;
}

IFileInfo ^ NetFbMetadbHandle::FileInfo()
{
    return gcnew NetFbFileInfo( (*pMetadbHandle_)->get_info_ref() );
}

String ^NetFbMetadbHandle::Path()
{
    assert( pMetadbHandle_ );

    const auto path = ( *pMetadbHandle_ )->get_path();
    return Convert::ToNet::ToValue( std::string_view{ path, strlen( path ) } );
}

double NetFbMetadbHandle::Length()
{
    assert( pMetadbHandle_ );
    return ( *pMetadbHandle_ )->get_length();
}

metadb_handle_ptr NetFbMetadbHandle::Handle()
{
    assert( pMetadbHandle_ );
    return *pMetadbHandle_;
}

} // namespace fooManagedWrapper
