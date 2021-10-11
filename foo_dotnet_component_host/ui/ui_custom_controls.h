#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace Qwr::DotnetHost
{

private
ref class HorizontalFillDockListView : ListView
{
private:
    int _tmpH = 0;
    bool _isFirstResize = true;
    bool _isScaledResize = false;
    bool _isResizeInProgress = false;

protected:
    void OnResize( EventArgs ^ e ) override;

    void ScaleControl( SizeF factor, BoundsSpecified specified ) override;
};

} // namespace Qwr::DotnetHost
