#pragma once

#include <variant>

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

generic<typename T> private ref class NetFbConfigVar sealed : public IConfigVar<T>
{
    using ConfigT = std::variant<cfg_bool, cfg_string, cfg_int_t<uint8_t>, cfg_int_t<uint32_t>, cfg_int_t<int8_t>, cfg_int_t<int32_t>>;

public:
    NetFbConfigVar( Guid ^ cfgGuid, T defaultValue );
    ~NetFbConfigVar();
    !NetFbConfigVar();

    virtual void Set( T value );
    virtual T Get();

private:
    T defaultValue_;
    ConfigT* pConfig_ = nullptr;
};

} // namespace Qwr::DotnetHost
