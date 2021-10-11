#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace Qwr::DotnetHost
{

ref class Preferences;
ref class ColumnSorter;

private
ref class PreferencesForm : public System::Windows::Forms::UserControl
{
public:
    PreferencesForm( Preferences ^ parent );

    bool HasNewComponents();

protected:
    ~PreferencesForm();

private:
    void Form_HandleCreated( Object ^ sender, EventArgs ^ e );
    void ColumnClick_EventHandler( Object ^ o, ColumnClickEventArgs ^ e );
    void ResizeListView( ListView ^ lv );

private:
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor->
    /// </summary>
    void InitializeComponent( void );

private:
    System::ComponentModel::Container ^ components;
    System::Windows::Forms::ListView ^ componentList;
    System::Windows::Forms::ColumnHeader ^ nameColumn;
    System::Windows::Forms::ColumnHeader ^ versionColumn;
    System::Windows::Forms::ColumnHeader ^ moduleColumn;

private:
    bool hasNewComponents_ = false;
    ColumnSorter ^ columnSorter_;
};
} // namespace Qwr::DotnetHost
