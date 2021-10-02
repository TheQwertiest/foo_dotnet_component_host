#include <stdafx.h>

#include "host.h"
#include <net_objects/fb_console.h>
#include <host/fb2k_controls.h>
#include <host/fb2k_utils.h>
#include <host/fb2k_services.h>
#include <loader/component_loader.h>

// TODO: replace WinForms with System.Drawing.Common

namespace Qwr::DotnetHost
{

 Host::Host()
{
}

Host ^ Host::GetInstance()
{
    if ( self_ == nullptr )
    {
        self_ = gcnew Host();
    }
    return self_;
}

void Host::Initialize( String ^ modulePath )
{
    modulePath_ = modulePath;

    try
    {
        fb2kControls_ = gcnew Fb2kControls();
        fb2kStaticServices_ = gcnew Fb2kStaticServices();
        fb2kDynamicServices_ = gcnew Fb2kDynamicServices();
        fb2kUtils_ = gcnew Fb2kUtils();

        auto componentLoader = gcnew ComponentLoader();

        pfc::hires_timer timer;
        timer.start();

        components_ = componentLoader->LoadComponentsInDir( System::IO::Path::GetDirectoryName( modulePath ) + "\\plugins", "foo_" );

        for each ( auto component in components_ )
        {
            try
            {
                //TODO: 
                component->Initialize( fb2kStaticServices_, fb2kUtils_ );
            }
            catch ( Exception ^ e )
            {
                auto msg = gcnew String( "Error loading .NET component:\n" );
                msg += "\tcomponent name: " + component->GetInfo().Name + "\n";
                msg += "\terror: " + e->Message;
                NetFbConsole::LogStatic( msg );
            }
        }

        // TODO: replace with delayed log
        FB2K_console_formatter() << 
            (DNET_NAME_WITH_VERSION " Components initialized in " + std::to_string(static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms").c_str();
    }
    catch ( Exception ^ e )
    {
        // TODO: replace with delayed popup
        NetFbConsole::LogStatic( e->Message );
    }
    
    isInitialized_ = true;
}

void Host::Start()
{
    fb2kUtils_->Initialize();

    // TODO: try catch here?
    for each ( auto pComponent in components_ )
    {
        pComponent->Start( fb2kDynamicServices_, fb2kControls_ );
    }
}

void Host::Shutdown()
{
    for each ( auto pComponent in components_ )
    {
        pComponent->Shutdown();
        delete pComponent;
    }

    delete components_;
    delete fb2kDynamicServices_;
    delete fb2kControls_;
}

bool Host::IsInitialized()
{
    return isInitialized_;
}

}

