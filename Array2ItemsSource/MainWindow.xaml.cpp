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
	winrt::com_array<int32_t> MainWindow::Names()
	{
		winrt::com_array<int32_t> values(2);

		values[0] = 1;
		values[1] = 2;

		return values;
	}
}
