#include <stdafx.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class Fb2kControls sealed : public IControls
{
public:
    Fb2kControls();

public:
    virtual IConsole ^ Console();
    virtual void ExecuteContextMenuCommand( String ^ command, List<IMetadbHandle ^> ^ metadbHandles );
    virtual void ExecuteMainMenuCommand( String ^ command );
    virtual IPlaybackControls ^ PlaybackControls();
    virtual ITitleFormat ^ TitleFormat( String ^ expression );

private:
    IConsole ^ pConsole_ = nullptr;
    IPlaybackControls ^ pPlaybackControls_ = nullptr;
};

} // namespace Qwr::DotnetHost
