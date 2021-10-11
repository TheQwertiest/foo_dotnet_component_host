#include <stdafx.h>

#include "ui_custom_controls.h"

namespace Qwr::DotnetHost
{

void HorizontalFillDockListView::OnResize( EventArgs ^ e )
{
    if ( _isResizeInProgress )
    {
        ListView::OnResize( e );
        return;
    }

    if ( _isFirstResize || _isScaledResize )
    {
        _tmpH = this->Height;
        if ( _isFirstResize )
        {
            this->Dock = DockStyle::Fill;
        }
    }

    ListView::OnResize( e );

    if ( !_isFirstResize && !_isScaledResize )
    {
        _isResizeInProgress = true;
        this->Height = _tmpH;
        _isResizeInProgress = false;
    }
    _isFirstResize = false;
}

void HorizontalFillDockListView::ScaleControl( SizeF factor, BoundsSpecified specified )
{
    if ( factor.Width != 1 || factor.Height != 1 )
    {
        this->Dock = DockStyle::None;
        _isScaledResize = true;
    }

    ListView::ScaleControl( factor, specified );
}

} // namespace Qwr::DotnetHost
