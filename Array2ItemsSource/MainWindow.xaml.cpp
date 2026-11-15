#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Array2ItemsSource::implementation
{
	winrt::com_array<winrt::hstring> MainWindow::Names()
	{
		winrt::com_array<winrt::hstring> values(2);

		values[0] = L"Item 1";
		values[1] = L"Item 2";

		return values;
	}
}
