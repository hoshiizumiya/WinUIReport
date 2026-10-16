#pragma once

#include "MainWindow.g.h"

namespace winrt::ItemCollectionOwnerLifetimeRepro::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void CaptureAndRelease_Click(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

        void ProbeCollection_Click(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

        void Reset_Click(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        void AppendLog(std::wstring_view text);
        void LogHResult(std::wstring_view operation, winrt::hresult_error const& error);
        void LogOwnerState();

        Microsoft::UI::Xaml::Controls::ListView m_owner{ nullptr };
        winrt::weak_ref<Microsoft::UI::Xaml::Controls::ListView> m_ownerWeak;
        Microsoft::UI::Xaml::Controls::ItemCollection m_items{ nullptr };
    };
}

namespace winrt::ItemCollectionOwnerLifetimeRepro::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
