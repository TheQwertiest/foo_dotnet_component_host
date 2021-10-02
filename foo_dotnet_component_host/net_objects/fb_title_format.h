#pragma once

using namespace System;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private
ref class NetFbTitleFormat sealed : public ITitleFormat
{
public:
    NetFbTitleFormat( String ^ expression );
    ~NetFbTitleFormat();
    !NetFbTitleFormat();

public:
    virtual String ^ Eval( bool force );
    virtual String ^ EvalWithMetadb( IMetadbHandle ^ metadbHandle );

private:
    titleformat_object::ptr* pTitleFormatObject_ = nullptr;
};

} // namespace Qwr::DotnetHost
