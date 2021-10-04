#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class NetFbFileInfo sealed : IFileInfo
{
public:
    NetFbFileInfo( metadb_info_container::ptr containerInfo );
    ~NetFbFileInfo();
    !NetFbFileInfo();

public:
    virtual IEnumerable<FileInfoInfo> ^ InfoEnum();
    virtual IEnumerable<FileInfoMeta> ^ MetaEnum();

    const file_info& FileInfo();

private:
    metadb_info_container::ptr* pContainerInfo_ = nullptr;
    const file_info& fileInfo_;
};

} // namespace Qwr::DotnetHost
