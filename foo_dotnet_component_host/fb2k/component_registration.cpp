#include <stdafx.h>

#include "component_registration.h"

#include <convert/to_native.h>

namespace
{

class ComponentVersion : public componentversion
{
public:
    ComponentVersion( const std::string& filename,
                      const std::string& name,
                      const std::string& version,
                      const std::string& description )
        : filename_( filename )
        , name_( name )
        , version_( version )
        , description_( description )
    {
    }
    void get_file_name( pfc::string_base& out ) override
    {
        out.set_string( filename_.data(), filename_.size() );
    }
    void get_component_name( pfc::string_base& out ) override
    {
        out.set_string( name_.data(), name_.size() );
    }
    void get_component_version( pfc::string_base& out ) override
    {
        out.set_string( version_.data(), version_.size() );
    }
    void get_about_message( pfc::string_base& out ) override
    {
        out.set_string( description_.data(), description_.size() );
    }

private:
    std::string filename_;
    std::string name_;
    std::string version_;
    std::string description_;
};

} // namespace

namespace
{

std::vector<std::unique_ptr<service_factory_single_t<ComponentVersion>>> g_registeredComponent;

}

namespace Qwr::DotnetHost
{

void RegisterComponent( String ^ filename, ComponentInfo ^ componentInfo )
{
    g_registeredComponent.emplace_back( std::make_unique<service_factory_single_t<ComponentVersion>>(
        Convert::ToNative::ToValue( filename ),
        Convert::ToNative::ToValue( componentInfo->Name ) + " (.NET)",
        Convert::ToNative::ToValue( componentInfo->Version->ToString() ),
        Convert::ToNative::ToValue( componentInfo->Description ) ) );
}

} // namespace Qwr::DotnetHost
