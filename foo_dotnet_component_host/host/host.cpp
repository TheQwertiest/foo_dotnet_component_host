#include <stdafx.h>

#include "host.h"

#include <convert/to_net.h>
#include <fb2k/component_registration.h>
#include <host/delayed_installation.h>
#include <host/fb2k_controls.h>
#include <host/fb2k_services.h>
#include <host/fb2k_utils.h>
#include <loader/component_loader.h>
#include <net_objects/fb_console.h>
#include <ui/ui_preferences.h>
#include <utils/delayed_log.h>

#include <component_paths.h>

using namespace System::IO;

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

        components_ = gcnew List<Component ^>;
        fb2kStaticServices_->RegisterPreferencesPage( Preferences::GetInfo(), Preferences::typeid );

        ProcessDelayedComponents();

        auto componentLoader = gcnew ComponentLoader();

        pfc::hires_timer timer;
        timer.start();

        auto components = componentLoader->GetComponentsInDir( Convert::ToNet::ToValue( NetComponentsDir() ) );
        for each ( auto component in components )
        {
            try
            {
                componentLoader->LoadComponent( component );
                component->info = component->instance->GetInfo();
                RegisterComponent( Path::GetFileNameWithoutExtension( component->underscoredName ), component->info );

                component->instance->Initialize( fb2kStaticServices_, fb2kUtils_ );

                components_->Add( component );
            }
            catch ( Exception ^ e )
            {
                auto msg = gcnew String( "Error loading .NET component:\n" );
                msg += "  component: " + component->underscoredName + "\n";
                msg += "  error: " + e->Message;
                DelayedLog( msg );
            }

            DelayedLog( gcnew String( "Loaded `" + component->underscoredName + "`" ), true, false );
        }

        DelayedLog( gcnew String( ( DNET_NAME_WITH_VERSION ": components loaded in " + std::to_string( static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms" ).c_str() ), false, false );
    }
    catch ( Exception ^ e )
    {
        DelayedLog( e->Message );
    }

    isInitialized_ = true;
}

void Host::Start()
{
    DrainDelayedLog();

    fb2kUtils_->Initialize();

    pfc::hires_timer timer;
    timer.start();

    for each ( auto component in components_ )
    {
        try
        {
            component->instance->Start( fb2kDynamicServices_, fb2kControls_ );
        }
        catch ( Exception ^ e )
        {
            auto msg = gcnew String( "Error during .NET component startup:\n" );
            msg += "  component: " + component->underscoredName + "\n";
            msg += "  error: " + e->Message;
            DelayedLog( msg );
        }
    }

    DelayedLog( gcnew String( ( DNET_NAME_WITH_VERSION ": components initialized in " + std::to_string( static_cast<uint32_t>( timer.query() * 1000 ) ) + "ms" ).c_str() ), false, false );
}

void Host::Shutdown()
{
    for each ( auto component in components_ )
    {
        component->instance->Shutdown();
        delete component->instance;
    }

    delete components_;
    delete fb2kDynamicServices_;
    delete fb2kControls_;
}

bool Host::IsInitialized()
{
    return isInitialized_;
}

List<Component ^> ^ Host::GetComponents()
{
    return components_;
}

} // namespace Qwr::DotnetHost
