#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Qwr::ComponentInterface;

namespace Qwr::DotnetHost
{

private
ref class NetFbMainMenuCommand sealed : public IMainMenuCommand
{
public:
    NetFbMainMenuCommand( Guid guid, String ^ name, String ^ description, Action ^ action );

    virtual property bool IsAlwaysHidden
    {
        bool get();
        void set( bool value );
    }
    virtual property bool IsChecked
    {
        bool get();
        void set( bool value );
    }
    virtual property bool IsDefaultHidden
    {
        bool get();
        void set( bool value );
    }
    virtual property bool IsEnabled
    {
        bool get();
        void set( bool value );
    }
    virtual property bool IsRadioChecked
    {
        bool get();
        void set( bool value );
    }

public:
    Guid ^ GetGuid();
    String ^ GetName();
    String ^ GetDescription();
    Action ^ GetAction();

private:
    Guid ^ guid_;
    String ^ name_;
    String ^ description_;
    Action ^ action_;

    bool isEnabled_ = true;
    bool isDefaultHidden_ = false;
    bool isAlwaysHidden_ = false;
    bool isChecked_ = false;
    bool isRadioChecked_ = false;
};

private
ref class NetFbMainMenuCommandSection sealed : public IMainMenuCommandSection
{
public:
    NetFbMainMenuCommandSection( Guid parentGuid, Nullable<uint16_t> sortPriority );

    virtual IMainMenuCommand ^ AddCommand( Guid guid, String ^ name, String ^ description, Action ^ action );

private:
    List<NetFbMainMenuCommand ^> ^ childCommands_;
};

private
ref class NetFbMainMenuGroup sealed : public IMainMenuGroup
{
public:
    NetFbMainMenuGroup( Guid guid );
    NetFbMainMenuGroup( Guid guid, Guid parentGuid, String ^ name, Nullable<uint16_t> sortPriority, bool isPopup );

    virtual IMainMenuGroup ^ AddGroup( Guid guid, String ^ name, Nullable<uint16_t> sortPriority, bool isPopup );
    virtual IMainMenuCommandSection ^ AddCommandSection( Nullable<uint16_t> sortPriority );

private:
    Guid guid_;

    List<NetFbMainMenuGroup ^> ^ childGroups_;
    List<NetFbMainMenuCommandSection ^> ^ childCommands_;
};

} // namespace Qwr::DotnetHost
