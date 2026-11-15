using Microsoft.UI.Xaml;

namespace TemplateRootCastRepro;

public partial class App : Application
{
    private Window? window;

    public App() => InitializeComponent();

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        window = new Window { Title = "TemplateRootCastRepro", Content = new ReproPage() };
        window.Activate();
    }
}
