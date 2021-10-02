#include <stdafx.h>
#include "fb_play_callback.h"

#include <fb2k/play_callback.h>

namespace Qwr::DotnetHost
{

NetFbPlayCallback::NetFbPlayCallback()
    : pImpl_( new PlayCallbackImpl(this) )
{
}

NetFbPlayCallback::~NetFbPlayCallback()
{
    this->!NetFbPlayCallback();
}

NetFbPlayCallback::!NetFbPlayCallback()
{
    if ( pImpl_ )
    {
        delete pImpl_;
        pImpl_ = nullptr;
    }
}

} // namespace Qwr::DotnetHost
