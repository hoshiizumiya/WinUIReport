#include "pch.h"
#include "MainWindow.xaml.h"

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Controls::Primitives;
using namespace Microsoft::UI::Xaml::Markup;

namespace winrt::ItemContainerGeneratorLifetimeRepro::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        AppendLog(L"Ready. Run the buttons in order.");
    }

    ItemsPanelTemplate MainWindow::CreateOldItemsPanelTemplate()
    {
        static constexpr wchar_t markup[] =
            LR"(<ItemsPanelTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><VirtualizingStackPanel /></ItemsPanelTemplate>)";

        return XamlReader::Load(hstring{ markup }).as<ItemsPanelTemplate>();
    }

    ItemsPanelTemplate MainWindow::CreateModernItemsPanelTemplate()
    {
        static constexpr wchar_t markup[] =
            LR"(<ItemsPanelTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><ItemsStackPanel /></ItemsPanelTemplate>)";

        return XamlReader::Load(hstring{ markup }).as<ItemsPanelTemplate>();
    }

    void MainWindow::AppendLog(std::wstring_view line)
    {
        std::wstring text{ Log().Text().c_str() };
        if (!text.empty())
        {
            text.append(L"\r\n");
        }

        text.append(line);
        Log().Text(text);
    }

    void MainWindow::AppendError(std::wstring_view operation, hresult_error const& error)
    {
        std::wstring line{ operation };
        line.append(L" failed: 0x");

        wchar_t hr[16]{};
        swprintf_s(hr, L"%08X", static_cast<uint32_t>(error.code().value));
        line.append(hr);

        if (!error.message().empty())
        {
            line.append(L" - ");
            line.append(error.message().c_str());
        }

        AppendLog(line);
    }

    void MainWindow::CreateGenerator_Click(
        [[maybe_unused]] IInspectable const& sender,
        [[maybe_unused]] RoutedEventArgs const& args)
    {
        m_removeAllFailed = false;
        m_generator = nullptr;

        m_listView = ListView();
        m_listView.ItemsPanel(CreateOldItemsPanelTemplate());

        m_listView.Items().Append(box_value(L"Item 0"));
        m_listView.Items().Append(box_value(L"Item 1"));
        m_listView.Items().Append(box_value(L"Item 2"));

        Host().Child(m_listView);
        m_listView.ApplyTemplate();
        m_listView.UpdateLayout();

        m_generator = m_listView.ItemContainerGenerator();

        if (!m_generator)
        {
            AppendLog(L"ItemContainerGenerator is null. The old-style panel path was not established.");
            return;
        }

        auto position = m_generator.GeneratorPositionFromIndex(0);

        std::wstring line =
            L"Created generator. GeneratorPositionFromIndex(0) = { Index = " +
            std::to_wstring(position.Index) +
            L", Offset = " +
            std::to_wstring(position.Offset) +
            L" }.";

        AppendLog(line);
    }

    void MainWindow::DetachGenerator_Click(
        [[maybe_unused]] IInspectable const& sender,
        [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (!m_listView || !m_generator)
        {
            AppendLog(L"Create the old-style generator first.");
            return;
        }

        // This transitions the ListView from the old VirtualizingStackPanel path
        // to the ModernCollectionBasePanel path. ItemsControl drops its old
        // ItemContainerGenerator and calls the internal DetachHost() on it.
        //
        // We intentionally keep m_generator alive to reproduce the public-object
        // lifetime after the host stops using that generator.
        m_listView.ItemsPanel(CreateModernItemsPanelTemplate());
        m_listView.UpdateLayout();

        auto current = m_listView.ItemContainerGenerator();

        if (!current)
        {
            AppendLog(L"Switched to ItemsStackPanel. ListView.ItemContainerGenerator is now null; the retained old generator is still alive.");
        }
        else if (current == m_generator)
        {
            AppendLog(L"Switched panel, but ListView still reports the same generator. Re-run after layout/template initialization.");
        }
        else
        {
            AppendLog(L"Switched to ItemsStackPanel. ListView now exposes a different generator; the retained old generator is still alive.");
        }
    }

    void MainWindow::ProbeDetached_Click(
        [[maybe_unused]] IInspectable const& sender,
        [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (!m_generator)
        {
            AppendLog(L"Create the generator first.");
            return;
        }

        try
        {
            auto position = m_generator.GeneratorPositionFromIndex(0);

            std::wstring line =
                L"Detached probe returned { Index = " +
                std::to_wstring(position.Index) +
                L", Offset = " +
                std::to_wstring(position.Offset) +
                L" }. The old public generator is still callable after host detach.";

            AppendLog(line);
        }
        catch (hresult_error const& error)
        {
            AppendError(L"GeneratorPositionFromIndex(0)", error);
        }
    }

    void MainWindow::RemoveAll_Click(
        [[maybe_unused]] IInspectable const& sender,
        [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (!m_generator)
        {
            AppendLog(L"Create the generator first.");
            return;
        }

        try
        {
            m_generator.RemoveAll();
            m_removeAllFailed = false;
            AppendLog(L"RemoveAll unexpectedly succeeded.");
        }
        catch (hresult_error const& error)
        {
            m_removeAllFailed = true;
            AppendError(L"RemoveAll", error);
            AppendLog(L"Important: RemoveAll has already cleared the internal item map before host-dependent preparation fails.");
        }
    }

    void MainWindow::TriggerCrash_Click(
        [[maybe_unused]] IInspectable const& sender,
        [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (!m_generator)
        {
            AppendLog(L"Create the generator first.");
            return;
        }

        if (!m_removeAllFailed)
        {
            AppendLog(L"Call RemoveAll after detaching first. This button is the intentional crash step.");
            return;
        }

        AppendLog(L"Calling GeneratorPositionFromIndex(0) after failed RemoveAll. Current WinUI can dereference the null internal item map here.");

        // Intentionally not wrapped in a C++ exception handler:
        // the bug is a native null dereference, not a projected HRESULT failure.
        auto position = m_generator.GeneratorPositionFromIndex(0);

        std::wstring line =
            L"Unexpectedly returned { Index = " +
            std::to_wstring(position.Index) +
            L", Offset = " +
            std::to_wstring(position.Offset) +
            L" }.";

        AppendLog(line);
    }
}
