#pragma once

using namespace System;
using namespace Qwr::ComponentBase;

namespace Qwr::DotnetHost
{

private
ref class NetFbPlayControl sealed : public IPlaybackControls
{
public:
    virtual IMetadbHandle^ NowPlaying();
    virtual double TrackPlaybackPosition();
    virtual bool IsPlaying();
    virtual bool IsPaused();
    virtual void Play();
    virtual void Pause();
    virtual void Next();
    virtual void Prev();
};

} // namespace fooManagedWrapper
