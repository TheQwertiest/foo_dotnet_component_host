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
ref class ComponentListEntry;

private
ref class PreferencesForm : public System::Windows::Forms::UserControl
{
public:
    PreferencesForm( Preferences ^ parent );

    bool HasComponentChanges();
    void Apply();

protected:
    ~PreferencesForm();

private:
    void Form_HandleCreated( Object ^ sender, EventArgs ^ e );
    void Form_HandleDestroyed( Object ^ sender, EventArgs ^ e );
    void ComponentList_MouseClick_EventHandler( Object ^ o, MouseEventArgs ^ e );
    void ComponentList_MouseDoubleClick_EventHandler( Object ^ o, MouseEventArgs ^ e );
    void ComponentList_ColumnClick_EventHandler( Object ^ o, ColumnClickEventArgs ^ e );
    void ComponentList_DragEnter_EventHandler( Object ^ sender, DragEventArgs ^ e );
    void ComponentList_DragDrop_EventHandler( Object ^ sender, DragEventArgs ^ e );
    void ComponentListMenu_About_EventHandler( Object ^ sender, EventArgs ^ e );
    void ComponentListMenu_Remove_EventHandler( Object ^ sender, EventArgs ^ e );
    void AdjustSizeComponentList( ListView ^ lv );

private:
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor->
    /// </summary>
    void InitializeComponent( void );

private:
    System::ComponentModel::Container ^ components;
    System::Windows::Forms::TableLayoutPanel ^ topTableLayout;
    System::Windows::Forms::ListView ^ componentList;
    System::Windows::Forms::ColumnHeader ^ nameColumn;
    System::Windows::Forms::ColumnHeader ^ versionColumn;
    System::Windows::Forms::ColumnHeader ^ moduleColumn;
    System::Windows::Forms::Label ^ leftLabel;
    System::Windows::Forms::Label ^ rightLabel;

private:
    Preferences ^ parent_ = nullptr;
    ColumnSorter ^ columnSorter_ = nullptr;
    ComponentListEntry ^ focusedItem_ = nullptr;

    bool hasComponentChanges_ = false;
    bool changesApplied_ = false;
};
} // namespace Qwr::DotnetHost
