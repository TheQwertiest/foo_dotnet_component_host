#include <stdafx.h>

#include "fb_config_var.h"

#include <convert/to_native.h>
#include <convert/to_net.h>

namespace
{

template <typename T1, typename T2>
Qwr::DotnetHost::NetFbConfigVar::ConfigT* InitializeCfg( const GUID& guid, const T2& defaultValue )
{
    return new Qwr::DotnetHost::NetFbConfigVar::ConfigT( std::in_place_type<T1>, guid, defaultValue );
}

} // namespace

namespace Qwr::DotnetHost
{

generic<typename T>
NetFbConfigVar<T>::NetFbConfigVar( Guid ^ cfgGuid, T defaultValue )
    : defaultValue_( defaultValue )
{
    if ( Object::ReferenceEquals( defaultValue, nullptr ) )
    {
        throw gcnew ArgumentNullException( "defaultValue" );
    }

    auto t = T::typeid;
    auto nativeGuid = Convert::ToNative::ToValue( cfgGuid );
    if ( t == bool ::typeid )
    {
        auto nativeDefaultValue = (bool)defaultValue;
        pConfig_ = InitializeCfg<cfg_bool>( nativeGuid, nativeDefaultValue );
    }
    else if ( t == String::typeid )
    {
        auto nativeDefaultValue = Convert::ToNative::ToValue( ( String ^ ) defaultValue );
        pConfig_ = InitializeCfg<cfg_string>( nativeGuid, nativeDefaultValue.c_str() );
    }
    else if ( t == uint8_t::typeid )
    {
        auto nativeDefaultValue = (uint8_t)defaultValue;
        pConfig_ = InitializeCfg<cfg_int_t<uint8_t>>( nativeGuid, nativeDefaultValue );
    }
    else if ( t == uint32_t::typeid )
    {
        auto nativeDefaultValue = (uint32_t)defaultValue;
        pConfig_ = InitializeCfg<cfg_int_t<uint32_t>>( nativeGuid, nativeDefaultValue );
    }
    else if ( t == int8_t::typeid )
    {
        auto nativeDefaultValue = (int8_t)defaultValue;
        pConfig_ = InitializeCfg<cfg_int_t<int8_t>>( nativeGuid, nativeDefaultValue );
    }
    else if ( t == int32_t::typeid )
    {
        auto nativeDefaultValue = (int32_t)defaultValue;
        pConfig_ = InitializeCfg<cfg_int_t<int32_t>>( nativeGuid, nativeDefaultValue );
    }
    else
    {
        throw gcnew ArgumentException( "Supplied generic parameter type is not supported" );
    }
}

generic<typename T>
    NetFbConfigVar<T>::~NetFbConfigVar()
{
    this->!NetFbConfigVar();
}

generic<typename T>
    NetFbConfigVar<T>::!NetFbConfigVar()
{
    if ( pConfig_ )
    {
        delete pConfig_;
        pConfig_ = nullptr;
    }
}

generic<typename T> void NetFbConfigVar<T>::Set( T value )
{
    assert( pConfig_ );
    if ( auto pAlternativeConfig = std::get_if<cfg_bool>( pConfig_ );
         pAlternativeConfig )
    {
        *pAlternativeConfig = (bool)value;
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_string>( pConfig_ );
              pAlternativeConfig )
    {
        *pAlternativeConfig = Convert::ToNative::ToValue( ( String ^ ) value ).c_str();
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<uint8_t>>( pConfig_ );
              pAlternativeConfig )
    {
        *pAlternativeConfig = (uint8_t)value;
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<uint32_t>>( pConfig_ );
              pAlternativeConfig )
    {
        *pAlternativeConfig = (uint32_t)value;
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<int8_t>>( pConfig_ );
              pAlternativeConfig )
    {
        *pAlternativeConfig = (int8_t)value;
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<int32_t>>( pConfig_ );
              pAlternativeConfig )
    {
        *pAlternativeConfig = (int32_t)value;
    }
    else
    {
        assert( false );
        throw gcnew Exception( "Internal error: not all variants were covered" );
    }
}

generic<typename T>
    T NetFbConfigVar<T>::Get()
{
    assert( pConfig_ );
    if ( auto pAlternativeConfig = std::get_if<cfg_bool>( pConfig_ );
         pAlternativeConfig )
    {
        return (T)pAlternativeConfig->get_value();
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_string>( pConfig_ );
              pAlternativeConfig )
    {
        const std::string str = static_cast<const char*>( *pAlternativeConfig );
        return (T)Convert::ToNet::ToValue( str );
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<uint8_t>>( pConfig_ );
              pAlternativeConfig )
    {
        return (T)pAlternativeConfig->get_value();
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<uint32_t>>( pConfig_ );
              pAlternativeConfig )
    {
        return (T)pAlternativeConfig->get_value();
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<int8_t>>( pConfig_ );
              pAlternativeConfig )
    {
        return (T)pAlternativeConfig->get_value();
    }
    else if ( auto pAlternativeConfig = std::get_if<cfg_int_t<int32_t>>( pConfig_ );
              pAlternativeConfig )
    {
        return (T)pAlternativeConfig->get_value();
    }
    else
    {
        assert( false );
        throw gcnew Exception( "Internal error: not all variants were covered" );
    }
}

} // namespace Qwr::DotnetHost
