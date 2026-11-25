#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/WinUI3Package.Svg.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::winuiessential_svg_lifetime::implementation
{
	void MainWindow::Create_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		{
			winrt::WinUI3Package::Svg::SvgImageSource source;

			// 只保存弱引用，不增加对象的强引用计数
			m_weakSource = winrt::make_weak(source);

			// 启动 HTTP 异步加载
			source.UriSource(winrt::Windows::Foundation::Uri{ L"http://127.0.0.1:8765/svg" });
			// source.UriSource(winrt::Windows::Foundation::Uri{ L"http://127.0.0.1:8765/release" });

		} // source 在此析构，释放外部持有的强引用

		ResultText().Text(L"External reference released.");
	}

	void MainWindow::Check_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		if (auto source = m_weakSource.get())
		{
			ResultText().Text(L"SvgImageSource is still alive.");
			OutputDebugStringW(L"SvgImageSource is still alive.\n");
		}
		else
		{
			ResultText().Text(L"SvgImageSource has been destroyed.");
			OutputDebugStringW(L"SvgImageSource has been destroyed.\n");
		}
	}
}
