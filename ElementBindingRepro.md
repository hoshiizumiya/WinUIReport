# C++/WinRT named-element x:Bind: forward and reverse code generation

Status: reproduction source prepared, not built or executed. Existing unrelated
BlankPageTest controls and dependencies are preserved.

`winui3xbindwithcontrol/BlankPageTest.xaml` contains two native ComboBox pairs.
Neither source element is exposed in BlankPageTest.idl. One pair uses OneWay;
the other uses TwoWay SelectedIndex and IsDropDownOpen. This removes LiquidGlass
and SwitchPresenter from the binding under investigation.

## Expected investigation

1. Compile the existing project manually. BlankPageTest is already a Page item.
2. Inspect BlankPageTest.xaml.g.hpp, not just the final C2039 diagnostic.
3. Compare the forward update for OneWaySource / TwoWaySource with the reverse
   callbacks for TwoWaySource.

The OpenNet generated output that motivated this repro uses:

```cpp
// Forward: implementation access, no projected getter required.
Update_LinkOpenBehaviorBox(
    winrt::get_self<implementation::RSSPage>(obj)->LinkOpenBehaviorBox(), phase);
// Reverse: projected access, fails without IDL getter.
GetDataRoot().LinkOpenBehaviorBox().SelectedIndex(obj47.SelectedIndex());
```

Hypothesis (not yet verified in this sample): the native TwoWay pair alone
produces the same projected-getter error. If it does not, this reduction is not
a confirmed repro; the next comparison should add the original presenter /
derived ComboBox structure, preserving the same SDK/compiler versions.

## Control experiment: IDL workaround

Add this read-only property inside BlankPageTest's runtimeclass:

```idl
Microsoft.UI.Xaml.Controls.ComboBox TwoWaySource { get; };
```

Do not add a manual C++ getter: the XAML-generated base already supplies it.
Compare the generated code and compilation result again. Alternatively change
both target TwoWay modes to OneWay to isolate reverse binding generation.
The sample deliberately starts without the workaround so the suspected failing
condition remains available to report upstream.

Microsoft documents the IDL requirement for element-to-element bindings:
https://learn.microsoft.com/en-us/windows/apps/develop/cpp-winrt/binding-property#element-to-element-binding
The question for an upstream report is why implementation access is generated
for the forward path but projected access for the reverse path, and whether the
compiler should handle both consistently or issue an earlier diagnostic.

## Separate nullable Boolean diagnostic

CheckBox.IsChecked is IReference<Boolean> (C# bool?), with false/true/null states.
IsEnabled is Boolean (C# bool), with only false/true states. A separate WMC1121
probe is `<Button IsEnabled="{x:Bind ACheckBox.IsChecked, Mode=OneWay}" />` plus
a named CheckBox. Keep it out of the ComboBox repro to avoid masking C2039.
An explicit conversion must define null handling, e.g. `value && value.Value()`
maps null and false to false. Testing only whether the reference exists is wrong:
a reference containing false is still non-null.
