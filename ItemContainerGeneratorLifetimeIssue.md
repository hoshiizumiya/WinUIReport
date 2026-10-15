# ItemContainerGenerator has an incomplete detached lifecycle after its ItemsControl host stops using it

## Summary

`ItemContainerGenerator` is already a public API, but its implementation assumes a host-bound lifetime that is not fully represented in the public contract.

A caller can retain an `ItemContainerGenerator` reference after its owning `ItemsControl` stops using that generator. The current detach path only clears the generator's weak reference to the host, while other internal state remains. Public methods then behave inconsistently: some continue to use stale generator state, while other methods require the host and can partially mutate the generator before returning an error.

This issue exists with the current sealed `ItemContainerGenerator`; derivation is not required to reproduce it.

## Current implementation

For the old-style items-panel path, `ItemsControl::InitializeItemContainerGenerator()` creates and owns an `ItemContainerGenerator`.

When `ItemsControl` transitions to a modern panel, the existing code removes the `ItemsChanged` handler, calls `ItemContainerGenerator::DetachHost()`, and clears the control's generator reference:

```cpp
if (m_tpGenerator)
{
    IFC(m_tpGenerator->remove_ItemsChanged(m_ItemsChangedToken));
    m_ItemsChangedToken = EventRegistrationToken();
    IFC(m_tpGenerator.Cast<ItemContainerGenerator>()->DetachHost());
    m_tpGenerator.Clear();
}
```

However, `DetachHost()` currently only does:

```cpp
HRESULT DetachHost()
{
    m_wrHost.Reset();
    return S_OK;
}
```

The `ItemContainerGenerator` can still be alive because application code may hold a strong WinRT reference to it. At that point the object is publicly callable, but its host is gone while its item map, recyclable-container state, view state, and other internal state are not transitioned to a defined detached state.

One concrete failure sequence is:

1. A valid generator is created while the `ItemsControl` uses an old-style panel.
2. Application code keeps a strong reference to the generator.
3. The `ItemsControl` switches to a modern panel and detaches the generator.
4. `RemoveAll()` is called on the retained generator.
5. `RemoveAllImpl()` clears the existing item map, then calls host-dependent grouping preparation.
6. Grouping preparation fails because `m_wrHost` no longer resolves.
7. The method returns an error after `m_pItemMap` has already been cleared.
8. A later call such as `GeneratorPositionFromIndex(0)` dereferences the now-null item map.

This means the problem is not only that a detached generator can return an error; a public call can leave the generator in a more invalid state and make a later public call crash.

## Reproduction

A C++/WinRT repro is available in:

`ItemContainerGeneratorLifetimeRepro/`

The sample intentionally does not depend on any unsealing change.

Steps:

1. Launch the sample.
2. Click **1. Create old-style generator**.
   - The sample creates a `ListView` with `VirtualizingStackPanel`.
   - It obtains and stores `ListView.ItemContainerGenerator`.
3. Click **2. Switch to modern panel**.
   - The sample changes the items panel to `ItemsStackPanel`.
   - WinUI transitions away from the old `ItemContainerGenerator` and calls its internal detach path.
   - The sample intentionally keeps the old generator alive.
4. Click **3. Probe detached generator**.
   - `GeneratorPositionFromIndex(0)` is called before further mutation.
   - This demonstrates that the retained object is still callable after detach.
5. Click **4. Call RemoveAll**.
   - The expected current behavior is an exception/HRESULT because host-dependent initialization can no longer complete.
6. Click **5. Call GeneratorPositionFromIndex again**.
   - On the current implementation this can dereference the item map that `RemoveAll()` cleared before failing and terminate the process.

The final step is intentionally separated because it may crash the repro process.

## Expected behavior

A public `ItemContainerGenerator` should have a defined lifetime after its host stops using it.

At minimum, one of the following contracts should be implemented consistently:

- the object enters a well-defined detached state and all host-dependent public APIs fail without mutating state; or
- detach fully resets/reinitializes the object so that all remaining public APIs have defined detached semantics.

A public method should never partially invalidate the object and make a later public method dereference null internal state.

## Actual behavior

The current object has an implicit host-bound lifetime but no complete detached-state contract.

`DetachHost()` clears only the host weak reference. Other state remains, and public methods make different assumptions about whether the host and item map are still valid.

`RemoveAll()` is especially problematic because it can clear the map before reaching a host-dependent operation that fails. This leaves the retained public object in a state where `GeneratorPositionFromIndex()` can dereference a null map.

## Why this matters

This is an existing public API lifetime issue, not only a derivation issue.

Application code is allowed to retain the object returned by `ItemsControl.ItemContainerGenerator`. The `ItemsControl` can later stop using that generator while the application still owns the WinRT reference.

The public API therefore needs a valid contract for the generator after detachment even if `ItemContainerGenerator` remains sealed.

This also affects any future work to make the class derivable: derivation would make the missing detached lifecycle visible at construction time as well, but the lifecycle gap already exists today.

## Possible direction

The implementation likely needs an explicit attached/detached invariant rather than individual null checks.

Possible pieces include:

- define a detached state for `ItemContainerGenerator`;
- make attach/detach transitions complete and transactional;
- ensure all public methods validate the required state before mutating internal structures;
- fully clean up or reinitialize item-map, view, event-subscription, recycling, grouping, and generation state during detach;
- consider exposing a read-only attachment/lifecycle state if callers need to know whether host-dependent operations are valid.

If derivation is supported later, `ItemsControl` can use the same lifecycle machinery to attach a derived generator instead of introducing a separate initialization path.

## Related

This was noticed while reviewing the work to make `ItemContainerGenerator` derivable. The constructor path exposed a null-state problem, but investigating it showed that the existing public object already has an incomplete lifetime after host detachment.
