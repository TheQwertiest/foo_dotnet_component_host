#include <acfu-sdk/acfu.h>

namespace qwr::acfu
{

// Ripped from acfu-sdk/utils/github.h:
// replaced rapidjson with nlohmann::json, because don't want to have
// multiple json implementations and additional submodule dependency

using n_json = nlohmann::json;

#define QWR_EXPECT_JSON( x ) \
    if ( !( x ) )            \
    throw std::logic_error( "unexpected JSON schema" )

class github_latest_release
    : public ::acfu::request
{
public:
    github_latest_release( const std::string& componentRepo,
                           const std::string& componentOwner )
        : componentRepo_( componentRepo )
        , componentOwner_( componentOwner )
    {
    }

    void run( file_info& info, abort_callback& abort ) override
    {
        pfc::string8 url = form_releases_url();
        http_request::ptr request = static_api_ptr_t<http_client>()->create_request( "GET" );

        for ( service_enum_t<::acfu::authorization> e; !e.finished(); ++e )
        {
            e.get()->authorize( url.get_ptr(), request, abort );
        }

        file::ptr response = request->run_ex( url.get_ptr(), abort );
        pfc::array_t<uint8_t> data;
        response->read_till_eof( data, abort );

        http_reply::ptr reply;
        if ( !response->cast( reply ) )
        {
            throw exception_service_extension_not_found();
        }

        n_json doc;
        try
        {
            doc = n_json::parse( std::string{ (const char*)data.get_ptr(), data.get_count() } );
        }
        catch ( const n_json::parse_error& err )
        {
            throw exception_io_data( PFC_string_formatter()
                                     << "error: " << err.what() );
        }

        pfc::string8 status;
        reply->get_status( status );
        // RFC: Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
        auto pos = status.find_first( ' ' );
        if ( ~0 == pos || 0 != pfc::strcmp_partial( status.get_ptr() + pos + 1, "200 " ) )
        {
            process_error( doc, status.get_ptr() );
        }
        else
        {
            process_response( doc, info );
        }
    }

private:
    pfc::string8 form_releases_url()
    {
        pfc::string8 url;
        url << "https://api.github.com/repos/" << componentOwner_.c_str()
            << "/" << componentRepo_.c_str() << "/releases/latest";
        return url;
    }

    void process_asset( const n_json& asset, file_info& info )
    {
        const auto assetRaw = asset.dump();
        info.info_set_ex( "asset", pfc_infinite, assetRaw.c_str(), assetRaw.length() );

        if ( auto it = asset.find( "browser_download_url" ); it != asset.end() && it->is_string() )
        {
            info.meta_set( "download_url", it.value().get<std::string>().c_str() );
        }
    }

    void process_release( const n_json& release, file_info& info )
    {
        {
            auto it = release.find( "tag_name" );
            QWR_EXPECT_JSON( it != release.end() && it->is_string() );
            info.meta_set( "version", it.value().get<std::string>().c_str() );
        }
        {
            auto it = release.find( "html_url" );
            QWR_EXPECT_JSON( it != release.end() && it->is_string() );
            info.meta_set( "download_page", it.value().get<std::string>().c_str() );
        }

        const auto releaseRaw = release.dump();
        info.info_set_ex( "release", pfc_infinite, releaseRaw.c_str(), releaseRaw.length() );
    }

    void process_response( const n_json& json, file_info& info )
    {
        QWR_EXPECT_JSON( json.is_object() );
        return process_release( json, info );
    }

    void process_error( const n_json& json, const char* http_status )
    {
        if ( auto it = json.find( "message" ); it != json.end() && it->is_string() )
        {
            throw exception_io_data( it.value().get<std::string>().c_str() );
        }
        else
        {
            throw exception_io_data( PFC_string_formatter()
                                     << "unexpected response; HTTP status: " << http_status );
        }
    }

private:
    const std::string componentRepo_;
    const std::string componentOwner_;
};

} // namespace qwr::acfu
