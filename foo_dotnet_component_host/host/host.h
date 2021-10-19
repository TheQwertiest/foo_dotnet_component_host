#pragma once

#include <host/component.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

ref class Fb2kControls;
ref class Fb2kStaticServices;
ref class Fb2kDynamicServices;
ref class Fb2kUtils;

private
ref class Host
{
public:
    Host();

    static Host ^ GetInstance();

public:
    void Initialize( String ^ modulePath );
    void Start();
    void Shutdown();

    bool IsInitialized();

    List<Component ^> ^ GetComponents();

    // TODO: remove the need for this
    Component ^ GetCurrentComponent();

private:
    static Host ^ self_;

    bool isInitialized_ = false;
    bool hasFailed_ = false;

    Component ^ currentComponent_ = nullptr;

    List<Component ^> ^ components_ = nullptr;
    Fb2kControls ^ fb2kControls_ = nullptr;
    Fb2kStaticServices ^ fb2kStaticServices_ = nullptr;
    Fb2kDynamicServices ^ fb2kDynamicServices_ = nullptr;
    Fb2kUtils ^ fb2kUtils_ = nullptr;

    String ^ modulePath_ = nullptr;
};

} // namespace Qwr::DotnetHost
