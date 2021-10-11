#pragma once

#include <commit_hash.h>
#include <version.h>

#define DNET_NAME            ".NET Component Host"
#define DNET_UNDERSCORE_NAME "foo_dotnet_component_host"
#define DNET_DLL_NAME        DNET_UNDERSCORE_NAME ".dll"

#define DNET_STRINGIFY_LITERAL( x ) #x
#define DNET_STRINGIFY( x )         DNET_STRINGIFY_LITERAL( x )

#ifdef DNET_VERSION_PRERELEASE_TEXT
#    define DNET_VERSION_PRERELEASE "-" DNET_VERSION_PRERELEASE_TEXT
#    define DNET_VERSION_METADATA   "+" DNET_STRINGIFY( DNET_COMMIT_HASH )
#else
#    define DNET_VERSION_PRERELEASE ""
#    define DNET_VERSION_METADATA   ""
#endif

#ifdef _DEBUG
#    define DNET_VERSION_DEBUG_SUFFIX " (Debug)"
#else
#    define DNET_VERSION_DEBUG_SUFFIX ""
#endif

#define DNET_VERSION                                                                  \
    DNET_STRINGIFY( DNET_VERSION_MAJOR )                                              \
    "." DNET_STRINGIFY( DNET_VERSION_MINOR ) "." DNET_STRINGIFY( DNET_VERSION_PATCH ) \
        DNET_VERSION_PRERELEASE DNET_VERSION_METADATA
#define DNET_NAME_WITH_VERSION DNET_NAME " v" DNET_VERSION DNET_VERSION_DEBUG_SUFFIX

#define DNET_ABOUT                              \
    DNET_NAME_WITH_VERSION " by TheQwertiest\n" \
                           "Build: " __TIME__ ", " __DATE__
