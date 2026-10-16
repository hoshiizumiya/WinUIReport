#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::ItemCollectionOwnerLifetimeRepro::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        AppendLog(L"Ready.");
        AppendLog(L"The important gate is the weak-reference check: only treat this as an ownerless repro when it reports EXPIRED.");
    }

    void MainWindow::AppendLog(std::wstring_view text)
    {
        std::wstring value = Log().Text().c_str();
        if (!value.empty())
        {
            value += L"\r\n";
        }
        value.append(text);
        Log().Text(value);
    }

    void MainWindow::LogHResult(std::wstring_view operation, hresult_error const& error)
    {
        wchar_t code[16]{};
        swprintf_s(code, L"0x%08X", static_cast<unsigned int>(error.code().value));

        std::wstring line(operation);
        line += L": FAILED ";
        line += code;
        if (!error.message().empty())
        {
            line += L" - ";
            line += error.message().c_str();
        }
        AppendLog(line);
    }

    void MainWindow::LogOwnerState()
    {
        auto owner = m_ownerWeak.get();
        AppendLog(owner ? L"Owner weak ref: ALIVE" : L"Owner weak ref: EXPIRED");
    }

    void MainWindow::CaptureAndRelease_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Log().Text(L"");
        m_items = nullptr;
        m_owner = nullptr;
        m_ownerWeak = {};

        m_owner = ListView();
        m_ownerWeak = make_weak(m_owner);
        m_items = m_owner.Items();

        AppendLog(L"Captured ListView.Items while the collection is empty.");
        AppendLog(L"Items.Size before releasing owner: " + std::to_wstring(m_items.Size()));
        LogOwnerState();

        // Drop the only application-held strong reference to the ItemsControl.
        // The ItemCollection is intentionally kept alive in m_items.
        m_owner = nullptr;
        AppendLog(L"Released the application's strong ListView reference.");
        LogOwnerState();

        // Give XAML/reference tracking one dispatcher turn before the explicit probe.
        DispatcherQueue().TryEnqueue([this]()
        {
            AppendLog(L"After one dispatcher turn:");
            LogOwnerState();
        });
    }

    void MainWindow::ProbeCollection_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_items)
        {
            AppendLog(L"No ItemCollection captured. Run step 1 first.");
            return;
        }

        auto owner = m_ownerWeak.get();
        if (owner)
        {
            AppendLog(L"Probe is INCONCLUSIVE: owner is still alive. Do not report this as an ownerless ItemCollection failure.");
            return;
        }

        AppendLog(L"Owner is confirmed expired. Probing the still-referenced ItemCollection...");

        try
        {
            AppendLog(L"Size(): OK, value = " + std::to_wstring(m_items.Size()));
        }
        catch (hresult_error const& error)
        {
            LogHResult(L"Size()", error);
        }

        try
        {
            m_items.Clear();
            AppendLog(L"Clear(): OK");
        }
        catch (hresult_error const& error)
        {
            LogHResult(L"Clear()", error);
        }

        try
        {
            m_items.Append(Button());
            AppendLog(L"Append(Button): OK");
        }
        catch (hresult_error const& error)
        {
            LogHResult(L"Append(Button)", error);
        }

        if (m_items.Size() > 0)
        {
            try
            {
                m_items.RemoveAt(0);
                AppendLog(L"RemoveAt(0): OK");
            }
            catch (hresult_error const& error)
            {
                LogHResult(L"RemoveAt(0)", error);
            }
        }
    }

    void MainWindow::Reset_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_items = nullptr;
        m_owner = nullptr;
        m_ownerWeak = {};
        Log().Text(L"");
        AppendLog(L"Reset complete.");
    }
}
