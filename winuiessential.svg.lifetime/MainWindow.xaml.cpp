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

	void MainWindow::Reproduce_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		winrt::WinUI3Package::Svg::SvgImageSource source;

		source.RasterizePixelWidth(200);
		source.RasterizePixelHeight(200);

		Preview().Source(source);

		source.StringSource(LR"(
        <svg xmlns="http://www.w3.org/2000/svg" width="200" height="200">
            <rect width="200" height="200" fill="red"/>
        </svg>
    )");

		// 上一次异步操作尚未完成，立即清空 Source。
		source.StringSource(L"");
		Preview().Source(source);
	}

	void MainWindow::Preview_ImageOpened(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		OutputDebugStringW(L"ImageOpened: stale SVG was rendered.\n");
	}
}
