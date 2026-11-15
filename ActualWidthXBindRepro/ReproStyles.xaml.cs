using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Diagnostics;

namespace ActualWidthXBindRepro;

public sealed partial class ReproStyles : ResourceDictionary
{
    public ReproStyles() => InitializeComponent();

    private void OnSourceSizeChanged(object sender, SizeChangedEventArgs args)
    {
        var source = (Border)sender;
        var caption = (TextBlock)((StackPanel)source.Parent).Children[1];
        Debug.WriteLine($"ActualWidth={args.NewSize.Width}; caption.MaxWidth={caption.MaxWidth}");
    }
}
