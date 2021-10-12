#pragma once

using namespace System;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class Fb2kUtils sealed : public IUtils
{
public:
    Fb2kUtils();
    ~Fb2kUtils();
    !Fb2kUtils();

    void Initialize();

    virtual Version ^ HostVersion();
    virtual Guid Fb2kGuid( Fb2kGuidId id );
    virtual Drawing::Icon ^ Fb2kIcon();
    virtual String ^ Fb2kPath();
    virtual String ^ Fb2kVersion();
    virtual bool IsFb2kMinimized();
    virtual String ^ ProfilePath();
    virtual void ShowPopup( String ^ text, String ^ title );

private:
    static_api_ptr_t<ui_control>* pUiControl_ = nullptr;
};

} // namespace Qwr::DotnetHost
