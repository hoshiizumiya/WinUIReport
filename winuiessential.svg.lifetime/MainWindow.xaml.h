#pragma once

#include "MainWindow.g.h"

using namespace winrt;

namespace winrt::winuiessential_svg_lifetime::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
        void Reproduce_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Preview_ImageOpened(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::winuiessential_svg_lifetime::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
