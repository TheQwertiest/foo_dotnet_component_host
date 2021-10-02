#pragma once

#include <convert/to_net.h>

namespace Qwr::DotnetHost
{

template <typename T, typename ... Args>
T^ GenerateException( std::string_view message,  Args ... args )
{
    return gcnew T( Convert::ToNet::ToValue( message ), args... );
}

template <typename T, typename... Args>
    T ^ GenerateException( pfc::exception e, Args... args ) {
        return gcnew T( Convert::ToNet::ToValue( e ), args... );
    }

}