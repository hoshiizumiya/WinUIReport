# Compiled template connector / custom DataTemplate root repro

Status: source prepared; not built or run. This is a reduction of the reported failure, not a confirmed reproduction yet.

Related upstream report: [WinUI #3916](https://github.com/microsoft/microsoft-ui-xaml/issues/3916) describes the same custom-control-root / compiled-ControlTemplate combination and internal elements reaching the outer connector. That report predates this Native AOT case, so the sample does not assume the issue is AOT-only. This sample remains unverified on the selected SDK.

## Run

Open the existing `../WinUIReport.slnx` in Visual Studio with WinUI and .NET 11 support. Set `TemplateRootCastRepro` as the startup project and use Debug / x64.
The package versions match the originating application: Windows App SDK 2.6.6-experimental, CsWinRT 2.3.1. Package restore needs a source providing those versions; no feed configuration is included.

Compare a normal managed Debug run with a native Debug publish:

```powershell
dotnet publish TemplateRootCastRepro.csproj -c Debug -r win-x64 -p:Platform=x64 -p:PublishAot=true -o artifacts/native
.rtifacts
ative\TemplateRootCastRepro.exe
```

Expected: a bordered card displays `One item` in both modes.
Reported original failure: native execution throws `InvalidCastException` / `0x80004002` in the generated DataTemplate `Bindings.Connect`, through `WinRT.CastExtensions.As<T>`.

## Two independent reductions

1. Wrap only the DataTemplate root in `Grid`, leaving the inner control and bindings unchanged:

```xml
<DataTemplate x:Key="RowTemplate" x:DataType="x:String">
    <Grid>
        <local:ReproCard Content="{x:Bind Mode=OneTime}" />
    </Grid>
</DataTemplate>
```

2. Starting from the original custom root, replace ONLY the control template's `Opacity="{x:Bind Opacity, Mode=OneWay}"` with `Opacity="{TemplateBinding Opacity}"`.

Record each result separately. Test 2 distinguishes a custom-root projection failure from interaction between the DataTemplate and ControlTemplate binding components. Connection IDs may legally repeat across templates; repetition alone does not establish a compiler bug.

Capture the actual failing `connectionId`, `target` runtime type, stack and generated `Connect` code before asserting which layer is responsible. A successful Grid workaround alone is not a general prohibition on custom DataTemplate roots.

No Toolkit, MVVM library, source-generated application properties, converters, images, DI or application business code is used.
