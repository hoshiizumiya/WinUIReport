# ItemCollection owner-lifetime repro

This project tests whether a public `Microsoft.UI.Xaml.Controls.ItemCollection` can outlive the `ItemsControl` that created it and, if so, whether the collection remains usable.

This is intentionally a **current-public-API** repro attempt. It does not depend on a public `ItemCollection` constructor or on making `ItemCollection` derivable.

## Why this matters

The native `CItemCollection` stores an `ItemsControl` owner pointer and several mutating operations require that owner. A proposed public constructor/derivation path would create an ownerless collection directly. Before filing a public bug for existing releases, this repro checks whether an ownerless `ItemCollection` is already reachable through the existing `ItemsControl.Items` API and owner lifetime.

## Steps

1. Build and run `ItemCollectionOwnerLifetimeRepro`.
2. Click **1. Capture Items + release owner**.
3. Wait for the `After one dispatcher turn` line.
4. Check `Owner weak ref`.
5. If it says **EXPIRED**, click **2. Probe ownerless collection**.

## Interpretation

### Reproduced

If the owner weak reference is `EXPIRED` while the held `ItemCollection` is still alive, and operations such as `Clear()` or `Append(Button)` fail, this demonstrates a current public API lifetime bug.

Please copy the exact HRESULT/message from the log into the issue.

### Not reproduced / inconclusive

If the weak reference remains `ALIVE`, the held `ItemCollection` is keeping the `ItemsControl` alive (directly or through XAML reference tracking). In that case this project does **not** establish an existing public lifetime bug. The ownerless behavior would still need to be addressed before exposing a standalone public `ItemCollection` constructor, but an issue should not claim the current `ItemsControl.Items` path is broken.

## Expected behavior if owner expires

A still-referenced WinRT collection object should either remain internally valid for its advertised collection operations, or the API/lifetime contract should explicitly prevent a detached collection from being observed. `Size()` succeeding while owner-dependent mutations fail would be especially useful evidence of a partially valid projected object.
