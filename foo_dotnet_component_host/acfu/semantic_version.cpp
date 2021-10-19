#include <stdafx.h>

#include "semantic_version.h"

#include <charconv>

namespace
{

std::string_view ExtractSuffixData( std::string_view& strView, char separator )
{
    std::string_view data;
    if ( size_t pos = strView.find_first_of( separator );
         std::string::npos != pos )
    {
        data = strView.substr( pos + 1 );
        strView.remove_suffix( strView.size() - pos );
    }
    return data;
}

template <typename T>
std::optional<T> GetNumber( std::string_view strView, int base = 10 )
{
    T number;
    if ( auto [pos, ec] = std::from_chars( strView.data(), strView.data() + strView.size(), number, base );
         ec == std::errc{} )
    {
        return number;
    }
    else
    {
        return std::nullopt;
    }
}

std::vector<std::string_view> SplitString( std::string_view input, char delim )
{
    std::vector<std::string_view> result;
    size_t found = input.find( delim );

    while ( found != std::string_view::npos )
    {
        result.emplace_back( input.substr( 0, found + 1 ) );
        input.remove_prefix( found + 1 );
        found = input.find( delim );
    }
    if ( input.empty() )
    {
        result.emplace_back( input );
    }

    return result;
}

} // namespace

namespace qwr
{

SemVer::SemVer( const std::string& strVer )
{
    const auto ret = ParseString( strVer );
    if ( !ret )
    {
        throw std::runtime_error( "Parsing failed" );
    }
    *this = *ret;
}

std::optional<SemVer> SemVer::ParseString( const std::string& strVer )
{
    SemVer semVer;

    std::string_view curScope( strVer );

    semVer.metadata = ExtractSuffixData( curScope, '+' );
    semVer.prerelease = ExtractSuffixData( curScope, '-' );

    if ( curScope.empty() )
    {
        return std::nullopt;
    }

    std::vector<std::optional<uint8_t>> versionNums;
    for ( const auto& splitView: ::SplitString( curScope, '.' ) )
    {
        const auto numRet = ::GetNumber<int8_t>( splitView );
        if ( !numRet )
        {
            return std::nullopt;
        }

        versionNums.emplace_back( numRet );
    }

    if ( versionNums.empty() || versionNums.size() > 3 )
    {
        return std::nullopt;
    }
    versionNums.resize( 3 );

    semVer.major = *versionNums[0];
    semVer.minor = versionNums[1].value_or( 0 );
    semVer.patch = versionNums[2].value_or( 0 );

    return semVer;
}

bool SemVer::operator==( const SemVer& other ) const
{ // metadata is ignored during comparison
    return ( major == other.major
             && minor == other.minor
             && patch == other.patch
             && prerelease == other.prerelease );
}
bool SemVer::operator!=( const SemVer& other ) const
{
    return ( !( *this == other ) );
}
bool SemVer::operator<( const SemVer& other ) const
{
    if ( major != other.major )
    {
        return ( major < other.major );
    }
    if ( minor != other.minor )
    {
        return ( minor < other.minor );
    }
    if ( patch != other.patch )
    {
        return ( patch < other.patch );
    }

    // metadata is ignored during comparison
    return IsPreleaseNewer( other.prerelease, prerelease );
}
bool SemVer::operator>( const SemVer& other ) const
{
    return ( other < *this );
}
bool SemVer::operator<=( const SemVer& other ) const
{
    return ( !( other < *this ) );
}
bool SemVer::operator>=( const SemVer& other ) const
{
    return ( !( *this < other ) );
}

bool SemVer::IsPreleaseNewer( std::string_view a, std::string_view b )
{
    if ( a == b )
    {
        return false;
    }

    if ( a.empty() || b.empty() )
    { // Pre-release versions have a lower precedence than the associated normal version
        return a.empty();
    }

    const auto isNumber = []( std::string_view str ) {
        return ( str.cend() == std::find_if_not( str.begin(), str.end(), []( char c ) { return std::isdigit( c ); } ) );
    };

    while ( !a.empty() && !b.empty() )
    {
        const std::string_view a_Token = ExtractSuffixData( a, '.' );
        const std::string_view b_Token = ExtractSuffixData( b, '.' );

        if ( a_Token != b_Token )
        {
            const bool a_isNumber = isNumber( a_Token );
            const bool b_isNumber = isNumber( b_Token );
            if ( a_isNumber != b_isNumber )
            { // Numeric identifiers always have lower precedence than non-numeric identifiers
                return !a_isNumber;
            }
            else if ( a_isNumber && b_isNumber )
            {
                auto numRet = ::GetNumber<int8_t>( a_Token );
                assert( numRet ); // should be valid, because of `isNumber` check
                const int8_t aNum = *numRet;

                numRet = ::GetNumber<int8_t>( a_Token );
                assert( numRet ); // should be valid, because of `isNumber` check
                const int8_t bNum = *numRet;

                assert( aNum != bNum ); // tokens would've been equal otherwise
                return aNum > bNum;
            }
            else
            {
                return a_Token > b_Token;
            }
        }
        else
        {
            if ( a.empty() != b.empty() )
            { // A larger set of pre-release fields has a higher precedence than a smaller set
                return !a.empty();
            }
        }
    }

    // They are equal (should not reach here)
    assert( 0 );
    return false;
}

} // namespace qwr
