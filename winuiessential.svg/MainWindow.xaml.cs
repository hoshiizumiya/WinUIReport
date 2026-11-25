using Microsoft.UI.Xaml;
using static System.Net.WebRequestMethods;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winuiessential.svg
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

        private void Reproduce_Click(object sender, RoutedEventArgs e)
        {
            WinUI3Package.Svg.SvgImageSource source;

            source.RasterizePixelWidth(200);
            source.RasterizePixelHeight(200);

            Preview().Source(source);

            source.StringSource(LR"(
                < svg xmlns = "http://www.w3.org/2000/svg" width = "200" height = "200" >
        
                    < rect width = "200" height = "200" fill = "red" />
        
                </ svg >
        
            )");

    // 上一次异步操作尚未完成，立即清空 Source。
    source.StringSource(L"");

        }

        private void Preview_ImageOpened(object sender, RoutedEventArgs e)
        {

        }
    }
}
