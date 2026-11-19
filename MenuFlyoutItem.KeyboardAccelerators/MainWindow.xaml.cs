using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace MenuFlyoutItem.KeyboardAccelerators
{
    /// <summary>
    /// The application window. This hosts a Frame that displays pages. Add your
    /// UI and logic to MainPage.xaml / MainPage.xaml.cs instead of here so you
    /// can use Page features such as navigation events and the Loaded lifecycle.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);

            AppWindow.SetIcon("Assets/AppIcon.ico");

            // Navigate the root frame to the main page on startup.
            RootFrame.Navigate(typeof(MainPage));
        }

        private async void ShowDialog_Click(object sender, RoutedEventArgs e)
        {
            ContentDialog content = new()
            {
                Title = "Reproduction",
                Content = "Close this dialog.",
                CloseButtonText = "Close"
            };

            WinUI3Package.WindowedContentDialog dialog = new(content);

            await dialog.ShowAsync(this, WinUI3Package.UnderlayMode.Smoke);
        }

        private async void Button_Loaded(object sender, RoutedEventArgs e)
        {
            // 5s after and trigger GC
            await Task.Delay(5000);
            GC.Collect();
        }
    }
}
