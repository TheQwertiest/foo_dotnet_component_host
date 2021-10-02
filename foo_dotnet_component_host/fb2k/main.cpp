#include <stdafx.h>

#include <net_objects/fb_console.h>
#include <host/host.h>

namespace
{

HINSTANCE g_hIns;

pfc::string_simple g_name, g_full_path;

bool g_services_available = false;
bool g_initialized = false;

} // namespace

using namespace Qwr::DotnetHost;

namespace core_api
{


HINSTANCE get_my_instance()
{
    return g_hIns;
}

HWND get_main_window()
{
    PFC_ASSERT( g_foobar2000_api );
    return g_foobar2000_api->get_main_window();
}
const char* get_my_file_name()
{
    return g_name;
}

const char* get_my_full_path()
{
    return g_full_path;
}

bool are_services_available()
{
    return g_services_available;
}
bool assert_main_thread()
{
    return ( g_services_available && g_foobar2000_api ? g_foobar2000_api->assert_main_thread() : true );
}

void ensure_main_thread()
{
    if ( !is_main_thread() )
    {
        uBugCheck();
    }
}

bool is_main_thread()
{
    return ( g_services_available && g_foobar2000_api ? g_foobar2000_api->is_main_thread() : true );
}
const char* get_profile_path()
{
    PFC_ASSERT( g_foobar2000_api );
    return g_foobar2000_api->get_profile_path();
}

bool is_shutting_down()
{
    return ( g_services_available && g_foobar2000_api ? g_foobar2000_api->is_shutting_down() : g_initialized );
}
bool is_initializing()
{
    return ( g_services_available && g_foobar2000_api ? g_foobar2000_api->is_initializing() : !g_initialized );
}
bool is_portable_mode_enabled()
{
    PFC_ASSERT( g_foobar2000_api );
    return g_foobar2000_api->is_portable_mode_enabled();
}

bool is_quiet_mode_enabled()
{
    PFC_ASSERT( g_foobar2000_api );
    return g_foobar2000_api->is_quiet_mode_enabled();
}
} // namespace core_api

namespace
{
class foobar2000_client_impl : public foobar2000_client
    , private foobar2000_component_globals
{
public:
    t_uint32 get_version() override
    {
        return FOOBAR2000_CLIENT_VERSION;
    }
    pservice_factory_base get_service_list() override
    {
        return service_factory_base::__internal__list;
    }

    void get_config( stream_writer* p_stream, abort_callback& p_abort ) override
    {
        cfg_var::config_write_file( p_stream, p_abort );
    }

    void set_config( stream_reader* p_stream, abort_callback& p_abort ) override
    {
        cfg_var::config_read_file( p_stream, p_abort );
    }

    void set_library_path( const char* path, const char* name ) override
    {
        g_full_path = path;
        g_name = name;
    }

    void services_init( bool val ) override
    {
        if ( val )
        {
            g_initialized = true;
        }
        g_services_available = val;
    }

    bool is_debug() override
    {
#ifdef _DEBUG
        return true;
#else
        return false;
#endif
    }
};

} // namespace

namespace 
    {

    ::foobar2000_client_impl g_client;

}

extern "C"
{
    __declspec( dllexport ) foobar2000_client* _cdecl foobar2000_get_interface( foobar2000_api* p_api, HINSTANCE hIns )
    {
        g_hIns = hIns;
        g_foobar2000_api = p_api;

        TCHAR buf[MAX_PATH];
        GetModuleFileName( hIns, buf, sizeof( buf ) );
        auto modulePath = gcnew String( buf );

        Host::GetInstance()->Initialize( modulePath );

        return &g_client;
    }
}
