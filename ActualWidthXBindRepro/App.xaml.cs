using Microsoft.UI.Xaml;

namespace ActualWidthXBindRepro;

public partial class App : Application
{
    private Window? window;

    public App() => InitializeComponent();

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        window = new Window { Title = "ActualWidthXBindRepro", Content = new ReproPage() };
        window.Activate();
    }
}
