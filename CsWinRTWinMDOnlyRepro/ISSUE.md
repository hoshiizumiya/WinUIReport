# CsWinRT 2.3.1: WinMD-only authoring emits ExportsType.g.cs referencing a missing WinRT.Module

## Describe the bug

When `CsWinRTComponent=true` and `CsWinRTGenerateWinMDOnly=true` are combined, CsWinRT generates `ExportsType.g.cs` even though WinMD-only mode skips component source generation. The generated wrapper references `global::WinRT.Module`, which is consequently missing. Compilation fails even with native activation exports disabled.

This was observed while building an interface-only contract library for a Native AOT plugin SDK. We only consume its WinMD in downstream projection projects; the contract assembly is not intended to be loaded as an activatable component.

## Steps to reproduce

Minimal standalone project: https://github.com/hoshiizumiya/WinUIReport/tree/master/CsWinRTWinMDOnlyRepro

Relevant properties:

```xml
<PropertyGroup>
    <TargetFramework>net11.0-windows10.0.26100.0</TargetFramework>
    <CsWinRTComponent>true</CsWinRTComponent>
    <CsWinRTGenerateWinMDOnly>true</CsWinRTGenerateWinMDOnly>
    <CsWinRTGenerateProjection>false</CsWinRTGenerateProjection>
    <CsWinRTAotExportsEnabled>false</CsWinRTAotExportsEnabled>
</PropertyGroup>
<ItemGroup>
    <PackageReference Include="Microsoft.Windows.CsWinRT" Version="2.3.1" />
    <CompilerVisibleProperty Include="CsWinRTGenerateWinMDOnly" />
</ItemGroup>
```

Run on Windows:

```powershell
dotnet build .\CsWinRTWinMDOnlyRepro.csproj -c Debug -p:Platform=x64 -tl:off
```

The original project's observed diagnostic was:

```text
ExportsType.g.cs(16,20): error CS0234: The type or namespace name
'Module' does not exist in the namespace 'WinRT'
(are you missing an assembly reference?)
```

The linked reduced project is prepared from the original configuration and source inspection; its standalone build and the workaround control have not yet been independently run on Windows.

## Expected behavior

WinMD-only generation should not emit an activation wrapper that depends on skipped component code. If this property combination is intentionally unsupported, a clear diagnostic or documented supported configuration would be preferable to invalid generated C#.

## Suspected cause

In [ComponentGenerator.Generate](https://github.com/microsoft/CsWinRT/blob/2.3.1.260716.1/src/Authoring/WinRT.SourceGenerator/Generator.cs#L183-L190), `GenerateSources()` is skipped in WinMD-only mode.

However, [SourceGenerator.Execute](https://github.com/microsoft/CsWinRT/blob/2.3.1.260716.1/src/Authoring/WinRT.SourceGenerator/Generator.cs#L537-L551) still calls `GenerateWinRTExportsType(context)` whenever `IsCsWinRTComponent()` is true. Its [generated wrapper](https://github.com/microsoft/CsWinRT/blob/2.3.1.260716.1/src/Authoring/WinRT.SourceGenerator/Generator.cs#L426-L465) calls:

```csharp
return global::WinRT.Module.GetActivationFactory(runtimeClassId);
```

The missing module is normally emitted by [cswinrt.exe in component mode](https://github.com/microsoft/CsWinRT/blob/2.3.1.260716.1/src/cswinrt/main.cpp#L365-L375).

Should the exports-type generation also be gated by `!context.ShouldGenerateWinMDOnly()` (and likewise native exports when enabled), or is there a different supported way to author a metadata-only contract using C#?

## Environment / workaround

- Microsoft.Windows.CsWinRT 2.3.1
- .NET 11, Windows target 10.0.26100.0, x64
- Original failure: Debug build during dependent Native AOT publish

Removing `CsWinRTGenerateWinMDOnly` and its CompilerVisibleProperty allows the complete component code generation path. This is the source-derived workaround; the post-fix Windows build still needs verification. No change to the authored interface or consumer ABI is intended.
