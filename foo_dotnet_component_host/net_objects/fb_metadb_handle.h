#pragma once

using namespace System;
using namespace System::Drawing;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private ref class NetFbMetadbHandle sealed: IMetadbHandle
{
public:
    NetFbMetadbHandle( const metadb_handle_ptr& metadbHandle );
    ~NetFbMetadbHandle();
    !NetFbMetadbHandle();

public:
    virtual Bitmap ^ Artwork( ArtId artIdb );
    virtual Bitmap ^ ArtworkStub( ArtId artId );
    virtual IFileInfo^ FileInfo();
    virtual double Length();
    virtual String^ Path();

internal:
    metadb_handle_ptr Handle();

private:
    metadb_handle_ptr* pMetadbHandle_ = nullptr;
};

}
