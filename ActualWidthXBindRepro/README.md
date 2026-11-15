# ActualWidth OneWay x:Bind / zero MaxWidth repro

Status: source prepared; not built or run. This tests the layout/binding behavior independently of Native AOT and custom template-root casts.

Open the existing `../WinUIReport.slnx` in Visual Studio with WinUI and .NET 11 support. Set `ActualWidthXBindRepro` as the startup project and use Debug / x64. Versions match the original report: Windows App SDK 2.6.6-experimental and CsWinRT 2.3.1; restore requires a source providing them.

1. Run with the debugger attached.
2. Check whether `Caption under the blue bar` is visible.
3. Click **Toggle width 160 / 240**.
4. Copy the Output lines reporting the source's actual width and the caption's `MaxWidth`.

Reported original behavior: the blue bar is laid out, but the caption has `MaxWidth=0` and stays invisible. OneWay does not refresh it when the source is arranged or resized.

Control experiments (change one thing at a time):

- Remove only `MaxWidth` from `ReproStyles.xaml`. The caption should appear.
- Alternatively add `caption.MaxWidth = args.NewSize.Width;` before the log statement in `OnSourceSizeChanged`. This uses SizeChanged to update the same target explicitly.

The sample has no DataTemplate, custom control class, converter, Toolkit, MVVM or application business code.

Documentation context for the report: Microsoft states that ActualWidth does not post binding change notifications and recommends SizeChanged:
https://learn.microsoft.com/windows/windows-app-sdk/api/winrt/microsoft.ui.xaml.frameworkelement.actualwidth

Thus this source demonstrates the reported zero-width behavior / diagnostic gap; the result alone does not establish a new undocumented framework regression. Include this documented limitation and the actual measurements in the report.

A native comparison, if needed, uses:

```powershell
dotnet publish ActualWidthXBindRepro.csproj -c Debug -r win-x64 -p:Platform=x64 -p:PublishAot=true -o artifacts/native
.rtifacts
ative\ActualWidthXBindRepro.exe
```

Related reports: [#1483](https://github.com/microsoft/microsoft-ui-xaml/issues/1483) discusses ActualWidth/Height binding and SizeChanged; [#8478](https://github.com/microsoft/microsoft-ui-xaml/issues/8478) is a separate C++/WinRT generated-code compilation failure, not this runtime observation.
