#include <stdafx.h>

#include "ui_preferences_form.h"

#include <convert/to_native.h>
#include <convert/to_net.h>
#include <host/delayed_installation.h>
#include <host/host.h>
#include <ui/ui_preferences.h>

#include <component_paths.h>

using namespace System::IO;
using namespace System::IO::Compression;
using namespace System::Windows;

namespace
{

void SetSortIcon( ListView ^ listView, int columnIndex, SortOrder order )
{
    const auto hColumnHeader = (HWND)SendMessage( (HWND)listView->Handle.ToPointer(), LVM_GETHEADER, NULL, NULL );

    for ( int columnNumber = 0; columnNumber <= listView->Columns->Count - 1; columnNumber++ )
    {
        HDITEM lvColumn{};
        lvColumn.mask = HDI_FORMAT;

        SendMessage( hColumnHeader, HDM_GETITEM, columnNumber, (LPARAM)&lvColumn );

        if ( !( order == SortOrder::None ) && columnNumber == columnIndex )
        {
            switch ( order )
            {
            case SortOrder::Ascending:
                lvColumn.fmt &= ~HDF_SORTDOWN;
                lvColumn.fmt |= HDF_SORTUP;
                break;
            case SortOrder::Descending:
                lvColumn.fmt &= ~HDF_SORTUP;
                lvColumn.fmt |= HDF_SORTDOWN;
                break;
            }
            lvColumn.fmt |= ( HDF_LEFT | HDF_BITMAP_ON_RIGHT );
        }
        else
        {
            lvColumn.fmt &= ~HDF_SORTDOWN & ~HDF_SORTUP & ~HDF_BITMAP_ON_RIGHT;
        }

        SendMessage( hColumnHeader, HDM_SETITEM, columnNumber, (LPARAM)&lvColumn );
    }
}

int CalculateColumnWidth( ColumnHeader ^ column, Font ^ font )
{
    column->AutoResize( ColumnHeaderAutoResizeStyle::ColumnContent );
    column->AutoResize( ColumnHeaderAutoResizeStyle::None );
    return Math::Max( column->Width, TextRenderer::MeasureText( column->Text, font ).Width + 10 );
}

} // namespace

namespace Qwr::DotnetHost
{

private
ref class ColumnSorter : System::Collections::IComparer
{
public:
    int sortColumn = 0;
    SortOrder sortOrder = SortOrder::None;

public:
    ColumnSorter()
    {
    }

    virtual int Compare( Object ^ x, Object ^ y )
    {
        try
        {
            auto lviX = ( ListViewItem ^ ) x;
            auto lviY = ( ListViewItem ^ ) y;

            int compareResult = listViewItemComparer->Compare( lviX->SubItems[sortColumn]->Text, lviY->SubItems[sortColumn]->Text );

            if ( sortOrder == SortOrder::Ascending )
            {
                return compareResult;
            }
            else if ( sortOrder == SortOrder::Descending )
            {
                return ( -compareResult );
            }
            else
            {
                return 0;
            }
        }
        catch ( Exception ^ )
        {
            return 0;
        }
    }

private:
    System::Collections::Comparer ^ listViewItemComparer = gcnew System::Collections::Comparer( Globalization::CultureInfo::CurrentUICulture );
};

private
ref class ComponentListEntry : ListViewItem
{
public:
    Component ^ component;
    bool isUpdated = false;

public:
    ComponentListEntry( Component ^ component )
        : ListViewItem( Generate( component ) )
        , component( component )
    {
    }

    void MarkAsToBeUpdated()
    {
        SubItems[0]->Text = "(unknown - please apply changes to load)";
        SubItems[1]->Text = "(unknown)";
        isUpdated = true;
    }

private:
    static array<String ^> ^ Generate( Component ^ component ) {
        return gcnew array<String ^>{ component->info ? component->info->Name : "",
                                      component->info ? component->info->Version->ToString() : "",
                                      component->underscoredName };
    }
};

} // namespace Qwr::DotnetHost

namespace Qwr::DotnetHost
{

PreferencesForm::PreferencesForm( Preferences ^ parent )
    : parent_( parent )
{
    this->Font = gcnew Drawing::Font( "MS Shell Dlg 2", 8 );

    InitializeComponent();

    columnSorter_ = gcnew ColumnSorter();
    componentList->ListViewItemSorter = columnSorter_;
    componentList->MouseClick += gcnew MouseEventHandler( this, &PreferencesForm::ComponentList_MouseClick_EventHandler );
    componentList->MouseDoubleClick += gcnew MouseEventHandler( this, &PreferencesForm::ComponentList_MouseDoubleClick_EventHandler );
    componentList->ColumnClick += gcnew ColumnClickEventHandler( this, &PreferencesForm::ComponentList_ColumnClick_EventHandler );
    componentList->DragEnter += gcnew DragEventHandler( this, &PreferencesForm::ComponentList_DragEnter_EventHandler );
    componentList->DragDrop += gcnew DragEventHandler( this, &PreferencesForm::ComponentList_DragDrop_EventHandler );
    this->HandleCreated += gcnew EventHandler( this, &PreferencesForm::Form_HandleCreated );
    this->HandleDestroyed += gcnew EventHandler( this, &PreferencesForm::Form_HandleDestroyed );
}

PreferencesForm::~PreferencesForm()
{
    if ( components )
    {
        delete components;
    }
}

bool PreferencesForm::HasComponentChanges()
{
    return hasComponentChanges_;
}

void PreferencesForm::Apply()
{
    changesApplied_ = true;
}

void PreferencesForm::Form_HandleCreated( Object ^ sender, EventArgs ^ e )
{
    auto components = Host::GetInstance()->GetComponents();

    AdjustSizeComponentList( componentList );

    componentList->BeginUpdate();
    for each ( auto component in components )
    {
        auto item = gcnew ComponentListEntry( component );
        componentList->Items->Add( item );
    }

    AdjustSizeComponentList( componentList );
    ComponentList_ColumnClick_EventHandler( componentList, gcnew ColumnClickEventArgs( 0 ) );

    componentList->EndUpdate();
}

void PreferencesForm::Form_HandleDestroyed( Object ^ sender, EventArgs ^ e )
{
    if ( !changesApplied_ )
    {
        ClearAllComponentDelayedStatuses();
    }
}

void PreferencesForm::ComponentList_MouseClick_EventHandler( Object ^ o, MouseEventArgs ^ e )
{
    if ( e->Button != Forms::MouseButtons::Right )
    {
        return;
    }

    auto item = componentList->HitTest( e->Location )->Item;
    if ( item == nullptr )
    {
        return;
    }

    focusedItem_ = ( ComponentListEntry ^ ) item;
    auto m = gcnew Forms::ContextMenuStrip();

    auto aboutItem = gcnew ToolStripMenuItem( "About" );
    if ( focusedItem_->isUpdated )
    {
        aboutItem->Enabled = false;
    }
    else
    {
        aboutItem->Font = gcnew Drawing::Font( aboutItem->Font, aboutItem->Font->Style | FontStyle::Bold );
        aboutItem->Click += gcnew EventHandler( this, &PreferencesForm::ComponentListMenu_About_EventHandler );
    }

    auto removeItem = gcnew ToolStripMenuItem( "Remove" );
    removeItem->Click += gcnew EventHandler( this, &PreferencesForm::ComponentListMenu_Remove_EventHandler );

    m->Items->Add( aboutItem );
    m->Items->Add( removeItem );

    m->Show( Cursor->Position );
}

void PreferencesForm::ComponentList_MouseDoubleClick_EventHandler( Object ^ o, MouseEventArgs ^ e )
{
    if ( e->Button != Forms::MouseButtons::Left )
    {
        return;
    }

    auto item = componentList->HitTest( e->Location )->Item;
    if ( item == nullptr )
    {
        return;
    }

    focusedItem_ = ( ComponentListEntry ^ ) item;
    if ( !focusedItem_->isUpdated )
    {
        ComponentListMenu_About_EventHandler( nullptr, nullptr );
    }
}

void PreferencesForm::ComponentList_ColumnClick_EventHandler( Object ^ sender, ColumnClickEventArgs ^ e )
{
    if ( e->Column == columnSorter_->sortColumn )
    {
        if ( columnSorter_->sortOrder == SortOrder::Ascending )
        {
            columnSorter_->sortOrder = SortOrder::Descending;
        }
        else
        {
            columnSorter_->sortOrder = SortOrder::Ascending;
        }
    }
    else
    {
        columnSorter_->sortColumn = e->Column;
        columnSorter_->sortOrder = SortOrder::Ascending;
    }

    componentList->Sort();
    SetSortIcon( componentList, columnSorter_->sortColumn, columnSorter_->sortOrder );
}

void PreferencesForm::ComponentList_DragEnter_EventHandler( Object ^ sender, DragEventArgs ^ e )
{
    e->Effect = DragDropEffects::None;
    if ( !e->Data->GetDataPresent( DataFormats::FileDrop ) )
    {
        return;
    }

    auto files = ( array<String ^> ^ ) e->Data->GetData( DataFormats::FileDrop );
    for each ( auto f in files )
    {
        if ( f->EndsWith( ".zip" ) || f->EndsWith( ".net-component" ) )
        {
            e->Effect = DragDropEffects::Copy;
            return;
        }
    }
}

void PreferencesForm::ComponentList_DragDrop_EventHandler( Object ^ sender, DragEventArgs ^ e )
{
    auto tmpUnpackDir = Convert::ToNet::ToValue( TempDir_ComponentUnpack() );

    auto componentsToInstall = gcnew List<String ^>();
    auto skippedFiles = gcnew HashSet<String ^>();

    auto files = ( array<String ^> ^ ) e->Data->GetData( DataFormats::FileDrop );
    for each ( auto f in files )
    {
        if ( !f->EndsWith( ".zip" ) && !f->EndsWith( ".net-component" ) )
        {
            skippedFiles->Add( f );
            continue;
        }

        if ( Directory::Exists( tmpUnpackDir ) )
        {
            Directory::Delete( tmpUnpackDir, true );
        }
        Directory::CreateDirectory( tmpUnpackDir );
        ZipFile::ExtractToDirectory( f, tmpUnpackDir );

        auto componentFiles = Directory::GetFiles( tmpUnpackDir, "dotnet_*.dll", SearchOption::AllDirectories );
        if ( componentFiles->Length == 0 )
        {
            skippedFiles->Add( f );
            continue;
        }

        auto componentFile = componentFiles[0];
        auto componentDir = Path::GetDirectoryName( componentFile );
        auto componentName = Path::GetFileNameWithoutExtension( componentFile );

        MarkComponentAsToBeInstalled( componentName, componentDir );
        componentsToInstall->Add( componentName );
    }

    if ( componentsToInstall->Count > 0 )
    {
        hasComponentChanges_ = true;
        parent_->Callback()->OnStateChanged();
    }

    for each ( ComponentListEntry ^ item in componentList->Items )
    {
        auto componentName = item->component->underscoredName;
        if ( componentsToInstall->Contains( componentName ) )
        {
            item->MarkAsToBeUpdated();
            componentsToInstall->Remove( componentName );
        }
    }

    for each ( auto componentName in componentsToInstall )
    {
        auto componentData = gcnew Qwr::DotnetHost::Component();
        componentData->underscoredName = componentName;

        auto item = gcnew ComponentListEntry( componentData );
        item->MarkAsToBeUpdated();
        componentList->Items->Add( item );
    }

    String ^ errorMsg = gcnew String( "" );
    for each ( auto componentName in skippedFiles )
    {
        errorMsg += "Could not load component \"" + componentName + "\": Unsupported format or corrupted file\n";
    }
    if ( !String::IsNullOrEmpty( errorMsg ) )
    {
        popup_message::g_show( Convert::ToNative::ToValue( errorMsg ).c_str(), DNET_UNDERSCORE_NAME );
    }

    AdjustSizeComponentList( componentList );
}

void PreferencesForm::ComponentListMenu_About_EventHandler( Object ^ sender, EventArgs ^ e )
{
    assert( focusedItem_ );
    popup_message::g_show( Convert::ToNative::ToValue( focusedItem_->component->info->Description ).c_str(),
                           Convert::ToNative::ToValue( "About " + focusedItem_->component->info->Name ).c_str() );
}

void PreferencesForm::ComponentListMenu_Remove_EventHandler( Object ^ sender, EventArgs ^ e )
{
    assert( focusedItem_ );

    hasComponentChanges_ = true;
    parent_->Callback()->OnStateChanged();

    MarkComponentAsToBeRemoved( focusedItem_->component->underscoredName );
    componentList->Items->Remove( focusedItem_ );
}

void PreferencesForm::AdjustSizeComponentList( ListView ^ lv )
{
    if ( lv == nullptr || lv->Columns->Count < 2 )
    {
        return;
    }

    const auto versionColumnSize = 75;

    lv->Columns[0]->Width = CalculateColumnWidth( lv->Columns[0], lv->Font ) + 10;
    lv->Columns[1]->Width = versionColumnSize + 10;
    lv->Columns[2]->Width = CalculateColumnWidth( lv->Columns[2], lv->Font ) + 10;

    bool scrollBarDisplayed = ( lv->Items->Count > 0 ) && ( lv->ClientSize.Height < ( lv->Items->Count + 1 ) * lv->Items[0]->Bounds.Height );
    auto scrollBarWidth = ( scrollBarDisplayed ? SystemInformation::VerticalScrollBarWidth : 0 );
    auto emptySpaceWidth = lv->ClientSize.Width - lv->Columns[0]->Width - lv->Columns[1]->Width - lv->Columns[2]->Width;

    if ( emptySpaceWidth > 0 )
    {
        auto additionalSpace = ( emptySpaceWidth - scrollBarWidth ) / 2;
        if ( emptySpaceWidth > 100 )
        {
            additionalSpace /= 2;
        }

        lv->Columns[0]->Width += additionalSpace;
        lv->Columns[1]->Width += additionalSpace;
    }
    else if ( ( lv->ClientSize.Width - lv->Columns[1]->Width - lv->Columns[2]->Width ) > 15 )
    {
        lv->Columns[2]->Width = lv->ClientSize.Width - lv->Columns[0]->Width - lv->Columns[1]->Width - scrollBarWidth;
    }
}

void PreferencesForm::InitializeComponent( void )
{
    this->topTableLayout = gcnew System::Windows::Forms::TableLayoutPanel();
    this->componentList = gcnew System::Windows::Forms::ListView();
    this->nameColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->versionColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->moduleColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->leftLabel = gcnew System::Windows::Forms::Label();
    this->rightLabel = gcnew System::Windows::Forms::Label();
    this->topTableLayout->SuspendLayout();
    this->SuspendLayout();

    //
    // topTableLayout
    //
    this->topTableLayout->AutoSize = true;
    this->topTableLayout->ColumnCount = 2;
    this->topTableLayout->ColumnStyles->Add( gcnew System::Windows::Forms::ColumnStyle() );
    this->topTableLayout->ColumnStyles->Add( gcnew System::Windows::Forms::ColumnStyle() );
    this->topTableLayout->Controls->Add( this->rightLabel, 1, 0 );
    this->topTableLayout->Controls->Add( this->leftLabel, 0, 0 );
    this->topTableLayout->Dock = System::Windows::Forms::DockStyle::Top;
    this->topTableLayout->Location = System::Drawing::Point( 0, 0 );
    this->topTableLayout->Margin = System::Windows::Forms::Padding( 0 );
    this->topTableLayout->Name = "topTableLayout";
    this->topTableLayout->Padding.All = 10;
    this->topTableLayout->RowCount = 1;
    this->topTableLayout->RowStyles->Add( gcnew System::Windows::Forms::RowStyle() );
    this->topTableLayout->Size = System::Drawing::Size( 584, 45 );
    this->topTableLayout->TabIndex = 4;
    //
    // componentList
    //
    this->componentList->AllowDrop = true;
    this->componentList->Columns->AddRange( gcnew cli::array<System::Windows::Forms::ColumnHeader ^>( 3 ){
        this->nameColumn,
        this->versionColumn,
        this->moduleColumn } );
    this->componentList->Dock = System::Windows::Forms::DockStyle::Top;
    this->componentList->FullRowSelect = true;
    this->componentList->HideSelection = false;
    this->componentList->Margin.All = 20;
    this->componentList->Margin.Top = 5;
    this->componentList->MultiSelect = false;
    this->componentList->Name = "componentList";
    this->componentList->Padding.All = 20;
    this->componentList->Padding.Top = 5;
    this->componentList->Size = System::Drawing::Size( 498, 420 );
    this->componentList->TabIndex = 1;
    this->componentList->UseCompatibleStateImageBehavior = false;
    this->componentList->View = System::Windows::Forms::View::Details;
    //
    // nameColumn
    //
    this->nameColumn->Text = "Name";
    //
    // versionColumn
    //
    this->versionColumn->Text = "Version";
    this->versionColumn->Width = 75;
    //
    // moduleColumn
    //
    this->moduleColumn->Text = "Module";
    //
    // leftLabel
    //
    this->leftLabel->AutoSize = true;
    this->leftLabel->Dock = System::Windows::Forms::DockStyle::Left;
    this->leftLabel->Font = gcnew Drawing::Font( this->Font, FontStyle::Bold );
    this->leftLabel->FlatStyle = System::Windows::Forms::FlatStyle::System;
    this->leftLabel->Location = System::Drawing::Point( 0, 0 );
    this->leftLabel->Margin = System::Windows::Forms::Padding( 0 );
    this->leftLabel->Name = "leftLabel";
    this->leftLabel->Padding = System::Windows::Forms::Padding( 0, 0, 0, 3 );
    this->leftLabel->TabIndex = 2;
    this->leftLabel->Text = "Installed components:";
    this->leftLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
    //
    // rightLabel
    //
    this->rightLabel->AutoSize = true;
    this->rightLabel->Dock = System::Windows::Forms::DockStyle::Right;
    this->rightLabel->FlatStyle = System::Windows::Forms::FlatStyle::System;
    this->rightLabel->Location = System::Drawing::Point( 323, 0 );
    this->rightLabel->Margin = System::Windows::Forms::Padding( 0 );
    this->rightLabel->Name = "rightLabel";
    this->rightLabel->Padding = System::Windows::Forms::Padding( 0, 0, 0, 3 );
    this->rightLabel->Size = System::Drawing::Size( 258, 25 );
    this->rightLabel->TabIndex = 2;
    this->rightLabel->Text = "Right-click a component for additional options.";
    this->rightLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
    //
    // Form1
    //
    this->AutoScaleDimensions = System::Drawing::SizeF( 7, 15 );
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->Controls->Add( this->componentList );
    this->Controls->Add( this->topTableLayout );
    this->Margin.All = 0;
    this->Name = "Form1";
    this->Size = System::Drawing::Size( 584, 578 );
    this->topTableLayout->ResumeLayout( false );
    this->topTableLayout->PerformLayout();
    this->ResumeLayout( false );
    this->PerformLayout();
}

} // namespace Qwr::DotnetHost
