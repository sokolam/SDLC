#include <windows.h>
#include "LoanModel.h"
#include "MainView.h"
#include "LoanController.h"

// Визуальные стили (Common Controls v6)
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow)
{
    LoanModel model;
    MainView view(model);
    model.AddObserver(&view);          // активная модель: вид подписан на модель
    LoanController controller(model, view);

    if (!view.Create(hInst, nCmdShow))
        return 1;

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
