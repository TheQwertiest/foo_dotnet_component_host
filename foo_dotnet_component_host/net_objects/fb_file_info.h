#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private ref class NetFbFileInfo sealed: IFileInfo
{
public:
    NetFbFileInfo( metadb_info_container::ptr containerInfo );
    ~NetFbFileInfo();
    !NetFbFileInfo();

public:
    virtual IEnumerable<IFileInfo::Info> ^ InfoEnum();
    virtual IEnumerable<IFileInfo::Meta> ^ MetaEnum();

// clang-format off
internal:
    const file_info& FileInfo();
    // clang-format on

private:
    metadb_info_container::ptr* pContainerInfo_ = nullptr;
    const file_info& fileInfo_;
};

}
