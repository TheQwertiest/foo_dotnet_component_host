#include <stdafx.h>

#include "fb_file_info.h"

#include <convert/to_net.h>
#include <net_objects/impl/enumerable_impl.h>

namespace Qwr::DotnetHost
{

ref class InfoEnumerator;
ref class MetaEnumerator;
ref class MetaValuesEnumerator;

} // namespace Qwr::DotnetHost

namespace Qwr::DotnetHost
{

private
ref class InfoEnumerator : IEnumerator<FileInfoInfo>
{
public:
    InfoEnumerator( NetFbFileInfo ^ netFileInfo );

    ~InfoEnumerator();

public:
    virtual bool MoveNext();

    virtual void Reset();

    virtual property FileInfoInfo Current
    {
        FileInfoInfo get();
    }

    // clang-format off
    virtual property Object ^ Current1 {
        Object ^ get() = Collections::IEnumerator::Current::get;
    }

private: 
    NetFbFileInfo ^ netFileInfo_;
    // clang-format on
    const file_info& fileInfo_;
    const int32_t infoCount_;

    int32_t curIndex_ = -1;
    FileInfoInfo curValue_{};
};

private
ref class MetaEnumerator : IEnumerator<FileInfoMeta>
{
public:
    MetaEnumerator( NetFbFileInfo ^ netFileInfo );

    ~MetaEnumerator();

public:
    virtual bool MoveNext();

    virtual void Reset();

    virtual property FileInfoMeta Current
    {
        FileInfoMeta get();
    }

    // clang-format off
    virtual property Object ^ Current1 {
        Object ^ get() = Collections::IEnumerator::Current::get;
    }

    const file_info& FileInfo();

private: 
    NetFbFileInfo ^ netFileInfo_;
    // clang-format on
    const file_info& fileInfo_;
    const int32_t metaCount_;

    int32_t curIndex_ = -1;
    FileInfoMeta curValue_{};
};

private
ref class MetaValuesEnumerator : IEnumerator<String ^>
{
public:
    MetaValuesEnumerator( MetaEnumerator ^ parent, int32_t index );

    ~MetaValuesEnumerator();

public:
    virtual bool MoveNext();

    virtual void Reset();

    virtual property String ^ Current {
        String ^ get();
    }

        // clang-format off
    virtual property Object ^ Current1 {
        Object ^ get() = Collections::IEnumerator::Current::get;
    }

        // clang-format off
private: 
    MetaEnumerator ^ parent_;
    // clang-format on
    const file_info& fileInfo_;
    const int32_t metaIndex_;
    const int32_t metaValueCount_;

    int32_t curIndex_ = -1;
    String ^ curValue_ = nullptr;
};

private
ref class InfoEnumFactory : IEnumeratorFactory<FileInfoInfo>
{
public:
    InfoEnumFactory( NetFbFileInfo ^ netFileInfo );

    virtual IEnumerator<FileInfoInfo> ^ Generate();

private:
    NetFbFileInfo ^ netFileInfo_;
};

private
ref class MetaEnumFactory : IEnumeratorFactory<FileInfoMeta>
{
public:
    MetaEnumFactory( NetFbFileInfo ^ netFileInfo );

    virtual IEnumerator<FileInfoMeta> ^ Generate();

private:
    NetFbFileInfo ^ netFileInfo_;
};

private
ref class MetaValuesEnumFactory : IEnumeratorFactory<String ^>
{
public:
    MetaValuesEnumFactory( MetaEnumerator ^ parent, int32_t index );

    virtual IEnumerator<String ^> ^ Generate();

private:
    MetaEnumerator ^ parent_;
    int32_t metaIndex_;
};

} // namespace Qwr::DotnetHost

namespace Qwr::DotnetHost
{

InfoEnumerator::InfoEnumerator( NetFbFileInfo ^ netFileInfo )
    : netFileInfo_( netFileInfo )
    , fileInfo_( netFileInfo_->FileInfo() )
    , infoCount_( fileInfo_.info_get_count() )
{
}

InfoEnumerator::~InfoEnumerator()
{
}

bool InfoEnumerator::MoveNext()
{
    if ( curIndex_ + 1 >= infoCount_ )
    {
        return false;
    }

    ++curIndex_;

    const auto name = fileInfo_.info_enum_name( curIndex_ );
    const auto value = fileInfo_.info_enum_value( curIndex_ );
    assert( name );
    assert( value );
    curValue_ = FileInfoInfo();
    curValue_.Name = Convert::ToNet::ToValue( std::string_view{ name } );
    curValue_.Value = Convert::ToNet::ToValue( std::string_view{ value } );

    return true;
}

void InfoEnumerator::Reset()
{
    curIndex_ = -1;
}

FileInfoInfo InfoEnumerator::Current::get()
{
    if ( curIndex_ < 0 || curIndex_ >= infoCount_ )
    {
        throw gcnew InvalidOperationException();
    }

    return curValue_;
}

Object ^ InfoEnumerator::Current1::get()
{
    return Current;
}

MetaEnumerator::MetaEnumerator( NetFbFileInfo ^ netFileInfo )
    : netFileInfo_( netFileInfo )
    , fileInfo_( netFileInfo_->FileInfo() )
    , metaCount_( fileInfo_.meta_get_count() )
{
}

MetaEnumerator::~MetaEnumerator()
{
}

bool MetaEnumerator::MoveNext()
{
    if ( curIndex_ + 1 >= metaCount_ )
    {
        return false;
    }

    ++curIndex_;

    const auto name = fileInfo_.info_enum_name( curIndex_ );
    const auto value = fileInfo_.info_enum_value( curIndex_ );
    assert( name );
    assert( value );
    curValue_ = FileInfoMeta();
    curValue_.Name = Convert::ToNet::ToValue( std::string_view{ name } );
    curValue_.Values = MakeEnumerable( gcnew MetaValuesEnumFactory( this, curIndex_ ) );

    return true;
}

void MetaEnumerator::Reset()
{
    curIndex_ = -1;
}

FileInfoMeta MetaEnumerator::Current::get()
{
    if ( curIndex_ < 0 || curIndex_ >= metaCount_ )
    {
        throw gcnew InvalidOperationException();
    }

    return curValue_;
}

Object ^ MetaEnumerator::Current1::get()
{
    return Current;
}

const file_info& MetaEnumerator::FileInfo()
{
    return fileInfo_;
}

MetaValuesEnumerator::MetaValuesEnumerator( MetaEnumerator ^ parent, int32_t index )
    : parent_( parent )
    , fileInfo_( parent->FileInfo() )
    , metaIndex_( index )
    , metaValueCount_( fileInfo_.meta_enum_value_count( metaIndex_ ) )
{
}

MetaValuesEnumerator::~MetaValuesEnumerator()
{
}

bool MetaValuesEnumerator::MoveNext()
{
    if ( curIndex_ + 1 >= metaValueCount_ )
    {
        return false;
    }

    ++curIndex_;

    const auto value = fileInfo_.meta_enum_value( metaIndex_, curIndex_ );
    assert( value );
    curValue_ = Convert::ToNet::ToValue( std::string_view{ value } );

    return true;
}

void MetaValuesEnumerator::Reset()
{
    curIndex_ = -1;
}

String ^ MetaValuesEnumerator::Current::get()
{
    if ( curIndex_ < 0 || curIndex_ >= metaValueCount_ )
    {
        throw gcnew InvalidOperationException();
    }

    return curValue_;
}

Object ^ MetaValuesEnumerator::Current1::get()
{
    return Current;
}

InfoEnumFactory::InfoEnumFactory( NetFbFileInfo ^ netFileInfo )
    : netFileInfo_( netFileInfo )
{
}

IEnumerator<FileInfoInfo> ^ InfoEnumFactory::Generate()
{
    return gcnew InfoEnumerator( netFileInfo_ );
}

MetaEnumFactory::MetaEnumFactory( NetFbFileInfo ^ netFileInfo )
    : netFileInfo_( netFileInfo )
{
}

IEnumerator<FileInfoMeta> ^ MetaEnumFactory::Generate()
{
    return gcnew MetaEnumerator( netFileInfo_ );
}

MetaValuesEnumFactory::MetaValuesEnumFactory( MetaEnumerator ^ parent, int32_t index )
    : parent_( parent )
    , metaIndex_( index )
{
}

IEnumerator<String ^> ^ MetaValuesEnumFactory::Generate()
{
    return gcnew MetaValuesEnumerator( parent_, metaIndex_ );
}

} // namespace Qwr::DotnetHost

namespace Qwr::DotnetHost
{

NetFbFileInfo::NetFbFileInfo( metadb_info_container::ptr containerInfo )
    : pContainerInfo_( new metadb_info_container::ptr( containerInfo ) )
    , fileInfo_( ( *pContainerInfo_ )->info() )
{
}

NetFbFileInfo::~NetFbFileInfo()
{
    this->!NetFbFileInfo();
}

NetFbFileInfo::!NetFbFileInfo()
{
    if ( !core_api::are_services_available() )
    {
        return;
    }

    if ( pContainerInfo_ )
    {
        delete pContainerInfo_;
        pContainerInfo_ = nullptr;
    }
}

IEnumerable<FileInfoInfo> ^ NetFbFileInfo::InfoEnum()
{
    return MakeEnumerable( gcnew InfoEnumFactory( this ) );
}

IEnumerable<FileInfoMeta> ^ NetFbFileInfo::MetaEnum()
{
    return MakeEnumerable( gcnew MetaEnumFactory( this ) );
}

const file_info& NetFbFileInfo::FileInfo()
{
    return fileInfo_;
}

} // namespace Qwr::DotnetHost
