# ItemContainerGeneratorLifetimeRepro

C++/WinRT repro for the existing `ItemContainerGenerator` host-lifetime problem.

The repro does not require `ItemContainerGenerator` to be derivable.

Run the five buttons in order:

1. Create a `ListView` with `VirtualizingStackPanel` and retain its `ItemContainerGenerator`.
2. Switch the `ListView` to `ItemsStackPanel`, causing the old generator to be detached internally while this sample keeps a strong reference to it.
3. Call `GeneratorPositionFromIndex(0)` on the retained generator.
4. Call `RemoveAll()`. The current implementation can clear the item map and then fail because the host is no longer available.
5. Call `GeneratorPositionFromIndex(0)` again. This is the intentional crash step: the current implementation can dereference the null item map.

The issue draft is in `../ItemContainerGeneratorLifetimeIssue.md`.
