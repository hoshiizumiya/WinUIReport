# ItemContainerGeneratorLifetimeRepro

Minimal C++/WinRT repro for the existing `ItemContainerGenerator` detached-lifetime problem.

This repro uses the current sealed public API. It does **not** require `ItemContainerGenerator` to be derivable.

## Build

Open `WinUIReport.slnx`, select `ItemContainerGeneratorLifetimeRepro`, and build/run an x64 Debug configuration with the required Windows App SDK runtime installed.

The project is unpackaged and uses `Microsoft.WindowsAppSDK` plus `Microsoft.Windows.CppWinRT` through `PackageReference`.

## Repro steps

Run the five buttons in order:

1. **Create old-style generator**
   - Creates a `ListView` using `VirtualizingStackPanel`.
   - Stores a strong reference to `ListView.ItemContainerGenerator`.
2. **Switch to modern panel**
   - Changes the panel to `ItemsStackPanel`.
   - `ItemsControl` stops using the old generator and invokes its internal `DetachHost()` path.
   - The sample deliberately keeps the old generator alive.
3. **Probe detached generator**
   - Calls `GeneratorPositionFromIndex(0)` before further mutation.
   - Demonstrates that the retained public object remains callable after host detach.
4. **Call RemoveAll**
   - The current implementation can clear the item map before host-dependent preparation fails because the host is no longer available.
5. **Call GeneratorPositionFromIndex again**
   - Intentional crash step.
   - The current implementation can dereference the null `m_pItemMap` left by the failed `RemoveAll()` call.

The crash step is separate so the state transition and the preceding HRESULT failure can be observed first.

## Why this repro matters

The bug is not specific to a proposed public constructor or derivation support. An application can retain the already-public generator returned by `ItemsControl.ItemContainerGenerator` while `ItemsControl` later detaches from that generator internally.

The issue draft is in `../ItemContainerGeneratorLifetimeIssue.md`.
