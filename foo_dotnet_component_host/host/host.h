#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

ref class Fb2kControls;
ref class Fb2kStaticServices;
ref class Fb2kDynamicServices;
ref class Fb2kUtils;

private ref class Host
{
public:
    Host();

    static Host ^ GetInstance();

public:
    void Initialize( String ^ modulePath );
    void Start();
    void Shutdown();

    bool IsInitialized();

private:
    static Host ^ self_;

    bool isInitialized_ = false;

    List<IComponent^> ^ components_ = nullptr;
    Fb2kControls ^ fb2kControls_ = nullptr;
    Fb2kStaticServices ^ fb2kStaticServices_ = nullptr;
    Fb2kDynamicServices ^ fb2kDynamicServices_ = nullptr;
    Fb2kUtils ^ fb2kUtils_ = nullptr;

    String ^ modulePath_ = nullptr;
};

} // namespace Qwr::DotnetHost
