#include <stdafx.h>

#include "ui_preferences_form.h"

#include <convert/to_native.h>
#include <convert/to_net.h>
#include <host/delayed_installation.h>
#include <host/host.h>
#include <ui/ui_custom_controls.h>
#include <ui/ui_preferences.h>

#include <component_paths.h>

using namespace System::IO;
using namespace System::IO::Compression;

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
    }

private:
    static array<String ^> ^ Generate( Component ^ component ) {
        return gcnew array<String ^>{ component->info->Name, component->info->Version->ToString(), component->underscoredName };
    }
};

} // namespace Qwr::DotnetHost

namespace Qwr::DotnetHost
{

PreferencesForm::PreferencesForm( Preferences ^ parent )
    : parent_( parent )
{
    InitializeComponent();

    columnSorter_ = gcnew ColumnSorter();
    componentList->ListViewItemSorter = columnSorter_;
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
    this->componentList = gcnew HorizontalFillDockListView();
    this->nameColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->versionColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->moduleColumn = gcnew System::Windows::Forms::ColumnHeader();
    this->SuspendLayout();
    //
    // componentList
    //
    this->componentList->AllowDrop = true;
    this->componentList->Columns->AddRange( gcnew cli::array<System::Windows::Forms::ColumnHeader ^>( 3 ){
        this->nameColumn,
        this->versionColumn,
        this->moduleColumn } );
    this->componentList->Dock = System::Windows::Forms::DockStyle::Fill;
    this->componentList->FullRowSelect = true;
    this->componentList->HideSelection = false;
    this->componentList->Location = System::Drawing::Point( 4, 3 );
    this->componentList->Margin = System::Windows::Forms::Padding( 4, 3, 4, 3 );
    this->componentList->MultiSelect = false;
    this->componentList->Name = "componentList";
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
    // Form1
    //
    this->AutoScaleDimensions = System::Drawing::SizeF( 7, 15 );
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->Controls->Add( this->componentList );
    this->Margin = System::Windows::Forms::Padding( 4, 3, 4, 3 );
    this->Name = "Form1";
    this->Size = System::Drawing::Size( 584, 578 );
    this->ResumeLayout( false );
}

} // namespace Qwr::DotnetHost
