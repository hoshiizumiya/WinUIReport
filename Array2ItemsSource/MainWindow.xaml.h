#pragma once

#include "MainWindow.g.h"

namespace winrt::Array2ItemsSource::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
		MainWindow() = default;

        winrt::com_array<int32_t> Names();
    };
}

namespace winrt::Array2ItemsSource::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
