#include <stdafx.h>
#include "fb2k_utils.h"

#include <convert/to_net.h>
#include <utils/fb2k_paths.h>
#include <host/host.h>

using namespace System::Reflection;
using namespace System::IO;

namespace Qwr::DotnetHost
{

 Fb2kUtils::Fb2kUtils()
{
}

Fb2kUtils::~Fb2kUtils()
{
    this->!Fb2kUtils();
}

Fb2kUtils::!Fb2kUtils()
{
    if ( pUiControl_ )
    {
        delete pUiControl_;
        pUiControl_ = nullptr;
    }
}

void Fb2kUtils::Initialize()
{
    assert( Host::GetInstance()->IsInitialized() );
    pUiControl_ = new static_api_ptr_t<ui_control>();
}

Version ^ Fb2kUtils::HostVersion()
{
    return Assembly::GetExecutingAssembly()->GetName()->Version;
}

Guid Fb2kUtils::Fb2kGuid( Fb2kGuidId id )
{
    switch ( id )
    {
    case Fb2kGuidId::PrefPage_Tagging:
        return Convert::ToNet::ToValue(preferences_page::guid_tagging);
    case Fb2kGuidId::PrefPage_Root:
        return Convert::ToNet::ToValue( preferences_page::guid_root );
    case Fb2kGuidId::PrefPage_Tools:
        return Convert::ToNet::ToValue(preferences_page::guid_tools);
    case Fb2kGuidId::PrefPage_Display:
        return Convert::ToNet::ToValue(preferences_page::guid_display);
    case Fb2kGuidId::PrefPage_Playback:
        return Convert::ToNet::ToValue(preferences_page::guid_playback);
    case Fb2kGuidId::PrefPage_Visualizations:
        return Convert::ToNet::ToValue(preferences_page::guid_visualisations);
    case Fb2kGuidId::PrefPage_Input:
        return Convert::ToNet::ToValue(preferences_page::guid_input);
    case Fb2kGuidId::PrefPage_Core:
        return Convert::ToNet::ToValue(preferences_page::guid_core);
    case Fb2kGuidId::PrefPage_TagWriting:
        return Convert::ToNet::ToValue(preferences_page::guid_tag_writing);
    case Fb2kGuidId::PrefPage_MediaLibrary:
        return Convert::ToNet::ToValue(preferences_page::guid_media_library);
    case Fb2kGuidId::PrefPage_Output:
        return Convert::ToNet::ToValue(preferences_page::guid_output);
    case Fb2kGuidId::PrefPage_Advanced:
        return Convert::ToNet::ToValue(preferences_page::guid_advanced);
    case Fb2kGuidId::PrefPage_Components:
        return Convert::ToNet::ToValue(preferences_page::guid_components);
    default:
        throw gcnew ArgumentOutOfRangeException();
    }
}

Drawing::Icon ^ Fb2kUtils::Fb2kIcon()
{
    if ( !Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "This method can be called only after component initialization" );
    }

    HICON hIcon = ( *pUiControl_ ).get_ptr()->get_main_icon();
    return Drawing::Icon::FromHandle( (IntPtr)hIcon );
}

String ^ Fb2kUtils::Fb2kPath()
{
    return Convert::ToNet::ToValue( Qwr::DotnetHost::Fb2kPath() ) + Path::DirectorySeparatorChar;
}

String ^ Fb2kUtils::Fb2kVersion()
{
    if ( !Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "This method can be called only after component initialization" );
    }

    const auto pChar = core_version_info_v2::get()->get_version_as_text();
    assert( pChar );

   return Convert::ToNet::ToValue(std::string_view(pChar));
}

String ^ Fb2kUtils::ProfilePath()
{
    if ( !Host::GetInstance()->IsInitialized() )
    {
        throw gcnew Exception( "This method can be called only after component initialization" );
    }

    return Convert::ToNet::ToValue( Qwr::DotnetHost::ProfilePath() ) + Path::DirectorySeparatorChar;
}

bool Fb2kUtils::IsFb2kMinimized()
{
    if ( !pUiControl_ )
    {
        throw gcnew Exception( "This method can be called only after component initialization" );
    }

    return ( *pUiControl_ ).get_ptr()->is_visible();
}

} // namespace Qwr::DotnetHost
