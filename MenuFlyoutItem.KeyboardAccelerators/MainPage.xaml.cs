using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace MenuFlyoutItem.KeyboardAccelerators
{
    /// <summary>
    /// The main content page displayed inside the application window.
    /// Add your UI logic, event handlers, and data binding here.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();

            // TODO: Add your initialization logic here.
        }

        private void TestButton1Click(object sender, RoutedEventArgs e)
        {
            TestButton4TeachingTip.IsOpen = true;
        }
    }
}
