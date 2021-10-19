#include <stdafx.h>

#include "acfu_registration.h"

#include <acfu/acfu_github.h>
#include <acfu/semantic_version.h>
#include <convert/to_native.h>

#include <acfu-sdk/utils/common.h>

namespace
{

class AcfuSource
    : public ::acfu::source
{
public:
    AcfuSource( const GUID& guid,
                const std::string& componentName,
                const std::string& componentFilename,
                const std::string& componentRepo,
                const std::string& componentOwner );

public:
    GUID get_guid() override;
    void get_info( file_info& info ) override;
    bool is_newer( const file_info& info ) override;
    ::acfu::request::ptr create_request() override;

private:
    std::string FetchVersion();

private:
    const GUID guid_;
    const std::string componentName_;
    const std::string componentFilename_;
    const std::string componentRepo_;
    const std::string componentOwner_;

    bool isVersionFetched_ = false;
    std::string installedVersion_;
};

} // namespace

namespace
{

AcfuSource::AcfuSource( const GUID& guid,
                        const std::string& componentName,
                        const std::string& componentFilename,
                        const std::string& componentRepo,
                        const std::string& componentOwner )
    : guid_( guid )
    , componentName_( componentName )
    , componentFilename_( componentFilename )
    , componentRepo_( componentRepo )
    , componentOwner_( componentOwner )
{
}

GUID AcfuSource::get_guid()
{
    return guid_;
}

void AcfuSource::get_info( file_info& info )
{
    if ( !isVersionFetched_ )
    {
        installedVersion_ = FetchVersion();
        isVersionFetched_ = true;
    }

    info.meta_set( "version", installedVersion_.c_str() );
    info.meta_set( "name", componentName_.c_str() );
    info.meta_set( "module", componentFilename_.c_str() );
}

bool AcfuSource::is_newer( const file_info& info )
{
    if ( !info.meta_get( "version", 0 ) || installedVersion_.empty() )
    {
        return false;
    }

    const auto availableVersion = [&info]() {
        std::string version = info.meta_get( "version", 0 );
        if ( version[0] == 'v' )
        {
            version = version.substr( 1 );
        }
        return version;
    }();

    try
    {
        return ( qwr::SemVer{ availableVersion } > qwr::SemVer{ installedVersion_ } );
    }
    catch ( const std::runtime_error& )
    {
        assert( false );
        return false;
    }
}

std::string AcfuSource::FetchVersion()
{
    auto cvRet = [&]() -> componentversion::ptr {
        for ( service_enum_t<componentversion> e; !e.finished(); ++e )
        {
            auto cv = e.get();

            pfc::string8 file_name;
            cv->get_file_name( file_name );
            if ( file_name.equals( componentFilename_.c_str() ) )
            {
                return cv;
            }
        }

        return componentversion::ptr{};
    }();

    if ( cvRet.is_empty() )
    {
        return "0.0.0";
    }
    else
    {
        pfc::string8 version;
        cvRet->get_component_version( version );
        return std::string( version.c_str(), version.length() );
    }
}

::acfu::request::ptr AcfuSource::create_request()
{
    return fb2k::service_new<qwr::acfu::github_latest_release>( componentRepo_, componentOwner_ );
}

} // namespace

namespace
{

std::vector<std::unique_ptr<service_factory_single_t<AcfuSource>>> g_registeredAcfu;

}

namespace Qwr::DotnetHost
{

void RegisterAcfu( Guid guid, String ^ componentName, String ^ componentFilename, String ^ componentRepo, String ^ repoOwner )
{
    g_registeredAcfu.emplace_back( std::make_unique<service_factory_single_t<AcfuSource>>(
        Convert::ToNative::ToValue( guid ),
        Convert::ToNative::ToValue( componentName ),
        Convert::ToNative::ToValue( componentFilename ),
        Convert::ToNative::ToValue( componentRepo ),
        Convert::ToNative::ToValue( repoOwner ) ) );
}

} // namespace Qwr::DotnetHost
