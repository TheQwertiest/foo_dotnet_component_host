#include <stdafx.h>

using namespace System::Reflection;

DECLARE_COMPONENT_VERSION( DNET_NAME, DNET_VERSION, DNET_ABOUT );

[assembly:AssemblyVersionAttribute( DNET_STRINGIFY( DNET_VERSION_MAJOR ) "." DNET_STRINGIFY( DNET_VERSION_MINOR ) ".*" )];
