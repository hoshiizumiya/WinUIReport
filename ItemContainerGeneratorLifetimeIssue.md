# ItemContainerGenerator can outlive its ItemsControl host and enter an invalid detached state

## Summary

`ItemContainerGenerator` is already a public API. Application code can retain the instance returned by `ItemsControl.ItemContainerGenerator` after the owning `ItemsControl` stops using that generator.

The current detach path does not establish a complete detached-state invariant. `ItemContainerGenerator::DetachHost()` only clears `m_wrHost`, while the public object and other internal state can remain alive. Public methods then make inconsistent assumptions about whether the host and item map still exist.

A concrete sequence can leave the generator partially mutated and make a later public API call dereference a null internal item map.

This is reproducible with the current sealed `ItemContainerGenerator`; making the class derivable is not required.

## Relevant implementation

For the legacy items-panel path, `ItemsControl::InitializeItemContainerGenerator()` creates an `ItemContainerGenerator` hosted by the `ItemsControl`.

When `ItemsControl` switches away from that path, it removes the event handler, detaches the generator, and releases its own reference:

```cpp
if (m_tpGenerator)
{
    IFC(m_tpGenerator->remove_ItemsChanged(m_ItemsChangedToken));
    m_ItemsChangedToken = EventRegistrationToken();
    IFC(m_tpGenerator.Cast<ItemContainerGenerator>()->DetachHost());
    m_tpGenerator.Clear();
}
```

`DetachHost()` itself only resets the weak host reference:

```cpp
HRESULT DetachHost()
{
    m_wrHost.Reset();
    return S_OK;
}
```

An application can still hold a strong WinRT reference to this same public object.

The problematic failure sequence is then:

1. `RemoveAll()` enters `RemoveAllImpl()` on the retained detached generator.
2. For the root generator, the recyclable-container collection is still present, so the initial cleanup can proceed.
3. `ClearBlocks(...)` clears the existing item-map structure and sets `m_pItemMap` to null.
4. `RemoveAllImpl()` then calls host-dependent grouping/view preparation.
5. That path requires `m_wrHost` to resolve to an `IGeneratorHost` and fails because the generator was detached.
6. `RemoveAll()` returns an error, but the object has already lost its item map.
7. A later `GeneratorPositionFromIndex(0)` assumes `m_pItemMap` is valid and can dereference the null pointer.

The important point is therefore not simply that an operation fails after detach. A public operation can fail **after partially invalidating the generator**, leaving a retained public WinRT object in a state where another public method can crash the process.

## Reproduction

A minimal C++/WinRT repro is prepared here:

`ItemContainerGeneratorLifetimeRepro/`

Repository branch:

`hoshiizumiya/WinUIReport`, branch `item-container-generator-lifetime-repro`

The sample does not depend on any `ItemContainerGenerator` unsealing change.

### Steps

1. Launch the sample.
2. Click **1. Create old-style generator**.
   - A `ListView` is created with `VirtualizingStackPanel`.
   - The sample obtains `ListView.ItemContainerGenerator` and keeps a strong reference.
3. Click **2. Switch to modern panel**.
   - The sample changes the panel to `ItemsStackPanel`.
   - `ItemsControl` stops using the old generator and invokes its internal detach path.
   - The sample intentionally keeps the old generator alive.
4. Click **3. Probe detached generator**.
   - `GeneratorPositionFromIndex(0)` is called before further mutation.
   - This shows that the retained public object is still callable after host detachment.
5. Click **4. Call RemoveAll**.
   - The call reaches host-dependent work after clearing the item map and returns an error because the host is gone.
6. Click **5. Call GeneratorPositionFromIndex again**.
   - This is intentionally separated as the crash step.
   - The current implementation can dereference the null `m_pItemMap` left by the failed `RemoveAll()` call.

## Expected behavior

A public `ItemContainerGenerator` should have a defined contract after its host stops using it.

At minimum, host detachment and all host-dependent public APIs should follow one consistent invariant. For example:

- detached generators reject host-dependent operations **before mutating internal state**; or
- detach fully transitions all dependent state to a valid detached representation whose public methods have defined semantics.

A public method should not fail after leaving the object in a state where a later public method can dereference null internal state.

## Actual behavior

The lifetime is implicitly host-bound, but the public WinRT object can outlive the host relationship.

`DetachHost()` clears only `m_wrHost`. Other generator state remains, and public methods do not consistently validate the detached state before using or mutating it.

In particular, `RemoveAll()` can clear the item map and only afterward discover that the required host is gone. `GeneratorPositionFromIndex()` can subsequently dereference that null map.

## Why this matters

This is an existing public API lifetime issue, independent of derivation.

Application code is allowed to retain the object returned by `ItemsControl.ItemContainerGenerator`. `ItemsControl` can later stop using that generator while the application still owns the WinRT reference. The retained public object therefore needs a safe and defined post-detach contract.

This also matters for any future attempt to make `ItemContainerGenerator` derivable. Direct construction would expose another form of unattached generator, but fixing only that construction path would not solve the existing detach lifecycle problem.

## Possible direction

The implementation likely needs an explicit attached/detached invariant rather than isolated null checks. Areas that may need to participate in the transition include:

- host weak reference;
- item map;
- current view and observable source state;
- event subscriptions;
- recyclable/shared recyclable container collections;
- grouping state and child-generator hierarchy;
- active generation state.

Host-dependent public methods should validate the required lifecycle state before making irreversible mutations.

If custom/derived generators are supported in the future, the same lifecycle machinery could also define how a generator is attached to and detached from an `ItemsControl`, instead of creating a separate partially initialized state.

## Related

This was found while investigating the work to make `ItemContainerGenerator` derivable. The new activation path exposed an unattached-construction problem, but source inspection showed that the current sealed public object already has an incomplete detached lifecycle.