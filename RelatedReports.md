# Related upstream reports

Checked 2026-11-15 against public GitHub issue bodies and comments. Links below are evidence of related reports, not confirmation that either new sample has reproduced on the current SDK.

| Local case | Report | Relevance |
|---|---|---|
| Custom DataTemplate root + compiled ControlTemplate | [WinUI #3916](https://github.com/microsoft/microsoft-ui-xaml/issues/3916), open | Very close: author observed control-template elements reaching the outer DataTemplate connector with overlapping IDs, causing InvalidCastException. The author explicitly says it occurs only when the custom control is the DataTemplate root. |
| ActualWidth binding | [WinUI #1483](https://github.com/microsoft/microsoft-ui-xaml/issues/1483), closed | Width/height bindings do not follow ActualWidth/Height; discussion points to SizeChanged and Toolkit bindable size extensions. Includes the documented no-change-notification behavior. |
| ActualWidth generated code | [WinUI #8478](https://github.com/microsoft/microsoft-ui-xaml/issues/8478), closed | Related API/compiler area, different failure: C++ generated code references unavailable Grid.ActualWidthProperty and fails compilation. Do not report the C# zero-MaxWidth sample as an exact duplicate. |
| LaunchGamePage ScrollView + composition | [WinUI #10974](https://github.com/microsoft/microsoft-ui-xaml/issues/10974), open | Very close: GetElementVisual on the ScrollView content causes a crash; changing to ScrollViewer avoids it. A comment reports the same result from Loaded, not only the constructor. |
| Other x:Bind custom control report | [CsWinRT #1949](https://github.com/microsoft/CsWinRT/issues/1949), open | UWP .NET 9 DataTemplate / UserControl report, but its reported cast is a null FileInformation data source. Related investigation only, not the same signature as our Connect target cast. |

## Submission notes

- Start with #3916 for the template connector sample; the existing issue already contains an older minimal repro attachment. Supply SDK/.NET versions and the new sample's actual result rather than claiming a new independent bug.
- For the width sample, include the ActualWidth documentation and the measured source/MaxWidth values. Separate a documented limitation from an unsupported-binding diagnostic request.
- #10974 is relevant to the separate ScrollView report; no third repro project has been added.
- Existing #10232 (selector TwoWay null writeback) and #7810 (a DataTemplate static function not running in a sample without Content) describe different mechanisms.
