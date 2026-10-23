#include "InputDialog.h"

namespace
{
    enum { ID_EDIT_RATE = 201, ID_EDIT_AMOUNT = 202, ID_EDIT_YEARS = 203 };
    const wchar_t* kClassName = L"LoanInputDialogWnd";

    struct State
    {
        HWND edits[3] = {};
        LoanInput* data = nullptr;
        InputDialog::Validator validator;
        bool done = false;
        bool accepted = false;
    };

    std::wstring GetText(HWND h)
    {
        int len = GetWindowTextLengthW(h);
        std::wstring s(len + 1, L'\0');
        GetWindowTextW(h, &s[0], len + 1);
        s.resize(len);
        return s;
    }

    HWND MakeChild(HWND parent, const wchar_t* cls, const wchar_t* text, DWORD style,
                   int x, int y, int w, int h, int id, DWORD exStyle = 0)
    {
        HWND c = CreateWindowExW(exStyle, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h,
                                 parent, (HMENU)(INT_PTR)id, GetModuleHandleW(nullptr), nullptr);
        SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        return c;
    }

    void OnOk(HWND hwnd, State* st)
    {
        LoanInput in;
        in.rate   = GetText(st->edits[0]);
        in.amount = GetText(st->edits[1]);
        in.years  = GetText(st->edits[2]);

        std::wstring err;
        if (st->validator && !st->validator(in, err))
        {
            MessageBoxW(hwnd, err.c_str(), L"Ошибка ввода", MB_OK | MB_ICONERROR);
            SetFocus(st->edits[0]);
            return;
        }
        *st->data = in;
        st->accepted = true;
        st->done = true;
    }

    LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        State* st;
        if (msg == WM_NCCREATE)
        {
            st = (State*)((CREATESTRUCTW*)lp)->lpCreateParams;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)st);
        }
        else
            st = (State*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

        switch (msg)
        {
        case WM_COMMAND:
            if (st && HIWORD(wp) == BN_CLICKED)
            {
                if (LOWORD(wp) == IDOK)          OnOk(hwnd, st);
                else if (LOWORD(wp) == IDCANCEL) { st->accepted = false; st->done = true; }
            }
            return 0;
        case WM_CLOSE:
            if (st) { st->accepted = false; st->done = true; }
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    void RegisterOnce()
    {
        static bool registered = false;
        if (registered) return;
        WNDCLASSW wc = {};
        wc.lpfnWndProc = DlgProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = kClassName;
        RegisterClassW(&wc);
        registered = true;
    }
}

bool InputDialog::Show(HWND owner, LoanInput& data, Validator validator)
{
    RegisterOnce();

    State st;
    st.data = &data;
    st.validator = std::move(validator);

    const int clientW = 380, clientH = 190;
    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRectEx(&rc, style, FALSE, WS_EX_DLGMODALFRAME);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;

    RECT orc; GetWindowRect(owner, &orc);
    int x = orc.left + ((orc.right - orc.left) - w) / 2;
    int y = orc.top + ((orc.bottom - orc.top) - h) / 2;

    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, kClassName, L"Ввод данных кредита", style,
                                x, y, w, h, owner, nullptr, GetModuleHandleW(nullptr), &st);
    if (!hwnd) return false;

    const wchar_t* labels[3] = { L"Процентная ставка (% годовых):", L"Сумма кредита:", L"Срок кредита (лет):" };
    const std::wstring* values[3] = { &data.rate, &data.amount, &data.years };
    for (int i = 0; i < 3; ++i)
    {
        int row = 22 + i * 38;
        MakeChild(hwnd, L"STATIC", labels[i], SS_LEFT, 15, row + 3, 190, 20, -1);
        st.edits[i] = MakeChild(hwnd, L"EDIT", values[i]->c_str(), WS_TABSTOP | ES_AUTOHSCROLL,
                                210, row, 155, 24, ID_EDIT_RATE + i, WS_EX_CLIENTEDGE);
    }
    MakeChild(hwnd, L"BUTTON", L"ОК", WS_TABSTOP | BS_DEFPUSHBUTTON, 95, 145, 90, 28, IDOK);
    MakeChild(hwnd, L"BUTTON", L"Отмена", WS_TABSTOP | BS_PUSHBUTTON, 195, 145, 90, 28, IDCANCEL);

    // Делаем окно модальным
    EnableWindow(owner, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    SetFocus(st.edits[0]);
    SendMessageW(st.edits[0], EM_SETSEL, 0, -1);

    MSG msg;
    while (!st.done && GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        if (!IsDialogMessageW(hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    if (!st.done) PostQuitMessage((int)msg.wParam); // пришёл WM_QUIT — пробросить дальше

    EnableWindow(owner, TRUE);
    DestroyWindow(hwnd);
    SetForegroundWindow(owner);
    return st.accepted;
}
