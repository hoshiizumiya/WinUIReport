# Draft issue: ItemCollection can remain projected after its ItemsControl owner is destroyed but owner-dependent operations fail

> Do not file this draft unless the repro reports `Owner weak ref: EXPIRED` and at least one collection operation fails. Replace placeholders with the exact log output first.

## Describe the bug

`ItemsControl.Items` returns an `ItemCollection`. If application code keeps that `ItemCollection` alive after the originating `ItemsControl` is destroyed, the projected collection can still be referenced, but owner-dependent operations fail.

The minimal repro verifies that the `ItemsControl` weak reference has expired before probing the collection, so this is not simply an operation against a still-live control.

## Steps to reproduce

Repro: https://github.com/hoshiizumiya/WinUIReport/tree/repro/itemcollection-owner-lifetime/ItemCollectionOwnerLifetimeRepro

1. Run `ItemCollectionOwnerLifetimeRepro`.
2. Click **1. Capture Items + release owner**.
3. Confirm the log reports `Owner weak ref: EXPIRED` after a dispatcher turn.
4. Click **2. Probe ownerless collection**.
5. Observe the failure from `Clear()` / `Append(Button)` / another operation.

## Actual behavior

Paste exact output here, for example:

```text
Owner weak ref: EXPIRED
Size(): OK, value = 0
Clear(): FAILED <HRESULT> - <message>
Append(Button): FAILED <HRESULT> - <message>
```

## Expected behavior

A still-referenced `ItemCollection` should not become a partially valid WinRT object solely because its originating `ItemsControl` has been destroyed. Either its advertised collection operations should remain valid for the detached lifetime, or the API should prevent exposing/retaining a collection whose implementation cannot operate without its owner.

## Additional context

The native implementation keeps an `ItemsControl` owner pointer and currently has owner-dependent mutation paths. This issue is about the observable public lifetime behavior, not about requesting a particular implementation fix.

Windows App SDK version: **replace with tested version**
Windows version: **replace with tested version**
Architecture: **replace with tested architecture**
