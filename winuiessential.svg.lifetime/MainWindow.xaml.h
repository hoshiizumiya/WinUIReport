#pragma once

#include "MainWindow.g.h"
#include <winrt/WinUI3Package.Svg.h>

using namespace winrt;

namespace winrt::winuiessential_svg_lifetime::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
	public:
		void Create_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

		void Check_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

	private:
		winrt::weak_ref<winrt::WinUI3Package::Svg::SvgImageSource> m_weakSource;
	};
}

namespace winrt::winuiessential_svg_lifetime::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
