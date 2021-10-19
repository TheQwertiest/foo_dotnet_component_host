#include <stdafx.h>

#include "preferences_pages.h"

#include <convert/to_native.h>
#include <net_objects/fb_preferences_page_callback.h>

namespace
{

using namespace Qwr;

class PreferencesPage : public preferences_page_v3
{
public:
    PreferencesPage( const std::string& name,
                     const GUID& guid,
                     const GUID& parentGuid,
                     const std::string& helpUrl );

public:
    void SetPreferencesPageType( Type ^ preferencePageType );

    preferences_page_instance::ptr instantiate( HWND parent, preferences_page_callback::ptr callback ) override;
    const char* get_name() override;
    bool get_help_url( pfc::string_base& p_out ) override;
    GUID get_guid() override;
    GUID get_parent_guid() override;

private:
    const std::string name_;
    const GUID guid_;
    const GUID parentGuid_;
    const std::string helpUrl_;

    gcroot<Type ^> preferencePageType_;
};

class PreferencesPageInstance : public preferences_page_instance
{
public:
    PreferencesPageInstance( preferences_page_callback::ptr callback );
    ~PreferencesPageInstance();

public:
    void Initialize( HWND parent, Type ^ preferencePageType );

    t_uint32 get_state() override;
    HWND get_wnd() override;
    void apply() override;
    void reset() override;

protected:
    const preferences_page_callback::ptr callback_;
    gcroot<IPreferencesPageCallback ^> netCallback_;
    gcroot<IPreferencesPage ^> pageImpl_;
};

} // namespace

namespace
{

using namespace Qwr::ComponentInterface;

PreferencesPage::PreferencesPage( const std::string& name, const GUID& guid, const GUID& parentGuid, const std::string& helpUrl /*, Type ^ preferencePageType */ )
    : name_( name )
    , guid_( guid )
    , parentGuid_( parentGuid )
    , helpUrl_( helpUrl )
{
}

void PreferencesPage::SetPreferencesPageType( Type ^ preferencePageType )
{
    preferencePageType_ = preferencePageType;
}

preferences_page_instance::ptr PreferencesPage::instantiate( HWND parent, preferences_page_callback::ptr callback )
{
    auto pInstance = fb2k::service_new<PreferencesPageInstance>( callback );
    pInstance->Initialize( parent, preferencePageType_ );
    return pInstance;
}

const char* PreferencesPage::get_name()
{
    return name_.c_str();
}

bool PreferencesPage::get_help_url( pfc::string_base& p_out )
{
    if ( helpUrl_.empty() )
    {
        return false;
    }

    p_out.set_string( helpUrl_.data(), helpUrl_.size() );
    return true;
}

GUID PreferencesPage::get_guid()
{
    return guid_;
}

GUID PreferencesPage::get_parent_guid()
{
    return parentGuid_;
}

PreferencesPageInstance::PreferencesPageInstance( preferences_page_callback::ptr callback )
    : callback_( callback )
{
    netCallback_ = gcnew DotnetHost::NetFbPreferencesPageCallback( callback );
}

PreferencesPageInstance::~PreferencesPageInstance()
{
    if ( !Object::ReferenceEquals( netCallback_, nullptr ) )
    {
        delete netCallback_;
    }
    delete netCallback_;
    delete pageImpl_;
}

void PreferencesPageInstance::Initialize( HWND parent, Type ^ preferencePageType )
{
    try
    {
        auto ctorInfo = preferencePageType->GetConstructor( Type::EmptyTypes );
        pageImpl_ = dynamic_cast<IPreferencesPage ^>( ctorInfo->Invoke( nullptr ) );
        pageImpl_->Initialize( IntPtr( parent ), netCallback_ );
    }
    catch ( Exception ^ e )
    {
        if ( e == nullptr || e->Message == nullptr )
        {
            throw std::runtime_error( "Preference page constructor failed: unknown error" );
        }
        else
        {
            throw std::runtime_error( Qwr::DotnetHost::Convert::ToNative::ToValue( e->Message ) );
        }
    }
}

t_uint32 PreferencesPageInstance::get_state()
{
    return static_cast<t_uint32>( pageImpl_->State() );
}
HWND PreferencesPageInstance::get_wnd()
{
    return (HWND)pageImpl_->Handle().ToPointer();
}
void PreferencesPageInstance::apply()
{
    pageImpl_->Apply();
}
void PreferencesPageInstance::reset()
{
    pageImpl_->Reset();
}

} // namespace

namespace
{

std::vector<std::unique_ptr<service_factory_single_t<PreferencesPage>>> g_registeredPreferences;

}

namespace Qwr::DotnetHost
{

void RegisterPreferencesPage( PreferencesPageInfo ^ preferencesPageInfo, Type ^ preferencePageType )
{
    if ( !preferencePageType->IsAssignableTo( IPreferencesPage::typeid ) )
    {
        throw gcnew ArgumentException( "Not compatible type", "preferencePageType" );
    }

    auto& pElem = g_registeredPreferences.emplace_back( std::make_unique<service_factory_single_t<PreferencesPage>>(
        Convert::ToNative::ToValue( preferencesPageInfo->Name ),
        Convert::ToNative::ToValue( preferencesPageInfo->Guid ),
        Convert::ToNative::ToValue( preferencesPageInfo->ParentGuid ),
        Convert::ToNative::ToValue( preferencesPageInfo->HelpUrl == nullptr ? gcnew String( "" ) : preferencesPageInfo->HelpUrl ) ) );
    pElem->get_static_instance().SetPreferencesPageType( preferencePageType );
};

} // namespace Qwr::DotnetHost
