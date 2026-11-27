# CsWinRT WinMD-only authoring emits an invalid exports wrapper

This standalone class library reduces an observed build failure in Snap Hutao's C#-first plugin contract project. It does not reference Windows App SDK, WinUI, the application, or its custom source generator. It is intentionally not part of the default solution build.

## Requirements

Windows, .NET 11 SDK, Windows SDK 10.0.26100.0 metadata, and Microsoft.Windows.CsWinRT 2.3.1. No Native AOT native linking or application activation is needed for the suspected compiler failure.

## Reproduce

From this folder in PowerShell:

```powershell
Remove-Item bin,obj -Recurse -Force -ErrorAction SilentlyContinue
dotnet build .\CsWinRTWinMDOnlyRepro.csproj -c Debug -p:Platform=x64 -tl:off
```

The project deliberately combines `CsWinRTComponent=true` and `CsWinRTGenerateWinMDOnly=true`, with the latter exposed through CompilerVisibleProperty. Native activation exports are explicitly disabled. The expected reproduction is CS0234 in ExportsType.g.cs because its call to global::WinRT.Module.GetActivationFactory references a missing generated type. EmitCompilerGeneratedFiles makes the generated wrapper inspectable under obj.

## Control / workaround

```powershell
Remove-Item bin,obj -Recurse -Force -ErrorAction SilentlyContinue
dotnet build .\CsWinRTWinMDOnlyRepro.csproj -c Debug -p:Platform=x64 -p:CsWinRTGenerateWinMDOnly=false -tl:off
Get-ChildItem bin -Recurse -Filter *.winmd
```

Full authoring should generate WinRT_Module.cs as well as ExportsType.g.cs. The contract assembly can be excluded from consumer references; consumers only need the generated WinMD.

## Evidence and status

- Observed in the original application project on Windows: ExportsType.g.cs(16,20), CS0234, namespace WinRT has no Module. The project contained only a public WinRT-compatible interface and used the same configuration, during a Debug win-x64 Native AOT publish of its dependent application.
- Source inspection of CsWinRT 2.3.1 confirms the two conflicting generation paths.
- This reduced project and its control build have not yet been executed independently on Windows. The error above is from the original project, not a fabricated log for this sample. Please record the actual command output before reporting the reduced result as confirmed.

See [the upstream issue draft](ISSUE.md) for the source links and expected behavior.
