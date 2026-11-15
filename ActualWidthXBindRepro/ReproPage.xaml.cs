using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ActualWidthXBindRepro;

public sealed partial class ReproPage : Page
{
    public ReproPage() => InitializeComponent();

    private void OnToggleWidth(object sender, RoutedEventArgs args)
    {
        Subject.Width = Subject.Width == 160 ? 240 : 160;
    }
}
