#pragma once

#include "MainWindow.g.h"

namespace winrt::ItemContainerGeneratorLifetimeRepro::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void CreateGenerator_Click(
            Microsoft::UI::Xaml::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void DetachGenerator_Click(
            Microsoft::UI::Xaml::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void ProbeDetached_Click(
            Microsoft::UI::Xaml::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void RemoveAll_Click(
            Microsoft::UI::Xaml::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void TriggerCrash_Click(
            Microsoft::UI::Xaml::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        Microsoft::UI::Xaml::Controls::ItemsPanelTemplate CreateOldItemsPanelTemplate();
        Microsoft::UI::Xaml::Controls::ItemsPanelTemplate CreateModernItemsPanelTemplate();

        void AppendLog(std::wstring_view line);
        void AppendError(std::wstring_view operation, winrt::hresult_error const& error);

        Microsoft::UI::Xaml::Controls::ListView m_listView{ nullptr };
        Microsoft::UI::Xaml::Controls::ItemContainerGenerator m_generator{ nullptr };
        bool m_removeAllFailed{ false };
    };
}

namespace winrt::ItemContainerGeneratorLifetimeRepro::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
