using System.Runtime.InteropServices;
using Windows.Foundation;

namespace CsWinRTWinMDOnlyRepro;

[Guid("C159AC51-721D-4E22-89C7-2C6158C62A0D")]
public interface IContract
{
    IAsyncOperation<string> LocateAsync();
}
