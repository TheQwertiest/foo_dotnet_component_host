#include <stdafx.h>

#include "menu_helpers.h"

#include <net_utils/exception_generator.h>

#pragma unmanaged

namespace
{

struct GuidHasher
{
    size_t operator()( const GUID& guid ) const
    {
        const uint64_t guid64_1 =
            ( static_cast<uint64_t>( guid.Data1 ) << 32 )
            | ( static_cast<uint64_t>( guid.Data2 ) << 16 )
            | guid.Data3;
        uint64_t guid64_2;
        memcpy( &guid64_2, guid.Data4, sizeof( guid.Data4 ) );

        std::hash<std::uint64_t> hash;
        return hash( guid64_1 ) ^ hash( guid64_2 );
    }
};

using GuidMenuMap = std::unordered_map<GUID, mainmenu_group::ptr, GuidHasher>;

bool DoesPathMatchCommand( const std::string& path, const std::string& command )
{
    const auto commandLen = command.length();
    const auto pathLen = path.length();

    if ( commandLen > pathLen )
    {
        return false;
    }

    if ( commandLen == pathLen )
    {
        return !_stricmp( command.c_str(), path.c_str() );
    }

    return ( ( path[pathLen - commandLen - 1] == '/' ) && !_stricmp( path.c_str() + pathLen - commandLen, command.c_str() ) );
}

contextmenu_node* FindContextCommandRecur( const std::string& p_command, std::string& basePath, contextmenu_node* p_parent )
{
    assert( p_parent && p_parent->get_type() == contextmenu_item_node::TYPE_POPUP );

    for ( size_t i = 0, max = p_parent->get_num_children(); i < max; ++i )
    {
        auto* pChild = p_parent->get_child( i );
        if ( !pChild )
        {
            continue;
        }

        auto curPath = basePath;
        curPath += pChild->get_name();

        switch ( pChild->get_type() )
        {
        case contextmenu_item_node::TYPE_POPUP:
        {
            curPath += '/';

            if ( auto retVal = FindContextCommandRecur( p_command, curPath, pChild );
                 retVal )
            {
                return retVal;
            }

            break;
        }
        case contextmenu_item_node::TYPE_COMMAND:
        {
            if ( DoesPathMatchCommand( curPath, p_command ) )
            {
                return pChild;
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }

    return nullptr;
}

/// @throw pfc::exception
bool ExecuteContextCommandUnsafe( const std::string& name, const metadb_handle_list& handles, uint32_t flags )
{
    auto cm = contextmenu_manager::get();
    if ( handles.get_count() )
    {
        cm->init_context( handles, flags );
    }
    else
    {
        cm->init_context_now_playing( flags );
    }

    auto pRoot = cm->get_root();
    if ( !pRoot || pRoot->get_type() != contextmenu_item_node::TYPE_POPUP )
    {
        return false;
    }

    std::string emptyPath;
    if ( auto retVal = FindContextCommandRecur( name, emptyPath, pRoot );
         retVal )
    {
        retVal->execute();
        return true;
    }

    return false;
}

GuidMenuMap GenerateGuidMainmenuMap()
{
    GuidMenuMap guidMap;
    for ( service_enum_t<mainmenu_group> e; !e.finished(); ++e )
    {
        const auto mmg = e.get();
        guidMap.try_emplace( mmg->get_guid(), mmg );
    }

    return guidMap;
}

std::string GenerateMainmenuCommandPath( const GuidMenuMap& group_guid_map, const service_ptr_t<mainmenu_commands>& ptr )
{
    std::string path;

    auto groupGuid = ptr->get_parent();
    while ( group_guid_map.count( groupGuid ) )
    {
        const auto& pGroup = group_guid_map.at( groupGuid );

        if ( mainmenu_group_popup::ptr pGroupPopup;
             pGroup->service_query_t( pGroupPopup ) )
        {
            pfc::string8_fast displayName;
            pGroupPopup->get_display_string( displayName );

            if ( !displayName.is_empty() )
            {
                path = fmt::format( "{}/{}", displayName, path );
            }
        }

        groupGuid = pGroup->get_parent();
    }

    return path;
}

mainmenu_node::ptr FindMainmenuCommandV2NodeRecur( mainmenu_node::ptr node, const std::string& basePath, const std::string& name )
{
    assert( node.is_valid() );

    if ( mainmenu_node::type_separator == node->get_type() )
    {
        return {};
    }

    auto curPath = basePath;

    pfc::string8_fast displayName;
    uint32_t tmp;
    node->get_display( displayName, tmp );
    if ( !displayName.is_empty() )
    {
        curPath += displayName.c_str();
    }

    switch ( node->get_type() )
    {
    case mainmenu_node::type_command:
    {
        if ( DoesPathMatchCommand( curPath, name ) )
        {
            return node;
        }
        break;
    }
    case mainmenu_node::type_group:
    {
        if ( curPath.back() != '/' )
        {
            curPath += '/';
        }

        for ( size_t i = 0, max = node->get_children_count(); i < max; ++i )
        {
            auto pChild = node->get_child( i );
            if ( auto retVal = FindMainmenuCommandV2NodeRecur( pChild, curPath, name );
                 retVal.is_valid() )
            {
                return retVal;
            }
        }
        break;
    }
    default:
    {
        assert( 0 );
    }
    }

    return {};
}

/// @throw pfc::exception
bool ExecuteMainMenuCommandUnsafe( const std::string& name )
{
    const auto group_guid_text_map = GenerateGuidMainmenuMap();

    for ( service_enum_t<mainmenu_commands> e; !e.finished(); ++e )
    {
        auto mmc = e.get();
        for ( size_t i = 0, max = mmc->get_command_count(); i < max; ++i )
        {
            auto path = GenerateMainmenuCommandPath( group_guid_text_map, mmc );
            if ( mainmenu_commands_v2::ptr mmc_v2;
                 mmc->service_query_t( mmc_v2 ) && mmc_v2->is_command_dynamic( i ) )
            { // dynamic items
                auto node = mmc_v2->dynamic_instantiate( i );
                if ( auto retVal = FindMainmenuCommandV2NodeRecur( node, path, name );
                     retVal.is_valid() )
                {
                    retVal->execute( nullptr );
                    return true;
                }

                continue;
            }
            else
            {                // static items
                pfc::string8_fast command;
                mmc->get_name( i, command );
                path += command;

                if ( DoesPathMatchCommand( path, name ) )
                {
                    mmc->execute( i, nullptr );
                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace

#pragma managed

using namespace System;

namespace Qwr::DotnetHost
{

void ExecuteContextMenuCommand( const std::string& name, const metadb_handle_list& handles, uint32_t flags )
{
    try
    {
        bool bRet = ExecuteContextCommandUnsafe( name, handles, flags );
        if ( !bRet )
        {
            throw GenerateException<ArgumentException>( "Unknown menu command: " + name );
        }
    }
    catch ( const pfc::exception& e )
    {
        throw GenerateException<Exception>( e );
    }
}

void ExecuteMainMenuCommand( const std::string& name )
{
    try
    {
        bool bRet = ExecuteMainMenuCommandUnsafe( name );
        if ( !bRet )
        {
            throw GenerateException<ArgumentException>( "Unknown menu command: " + name );
        }
    }
    catch ( const pfc::exception& e )
    {
        throw GenerateException<Exception>( e );
    }
}

} // namespace Qwr::DotnetHost
