#pragma once

namespace Qwr::DotnetHost
{

    template <typename T>
private interface class IEnumeratorFactory
{
    virtual IEnumerator<T> ^ Generate();
};

template <typename T>
private ref class EnumerableImpl : IEnumerable<T>
{
public:
    EnumerableImpl( IEnumeratorFactory<T> ^ enumFactory )
        : enumFactory_( enumFactory )
    {
    }

    virtual IEnumerator<T> ^ GetEnumerator() {
        // FIX: replace with factory
        return enumFactory_->Generate();
    }

        // clang-format off
internal:
    virtual Collections::IEnumerator^ GetEnumerator1() = Collections::IEnumerable::GetEnumerator {
        return GetEnumerator();
    };

    // clang-format on

private:
    IEnumeratorFactory<T> ^ enumFactory_;
};

template <typename T>
    EnumerableImpl<T> ^ MakeEnumerable( IEnumeratorFactory<T> ^ enumFactory ) {
        return gcnew EnumerableImpl<T>( enumFactory );
    }

} // namespace Qwr::DotnetHost
