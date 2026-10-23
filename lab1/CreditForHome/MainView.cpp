#include "MainView.h"
#include <cstdio>

namespace { const int ID_BTN_ENTER = 101; const wchar_t* kMainClass = L"MortgageMainWnd"; }

bool MainView::Create(HINSTANCE hInst, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.lpszClassName = kMainClass;
    if (!RegisterClassW(&wc)) return false;

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rc = { 0, 0, 470, 330 };
    AdjustWindowRect(&rc, style, FALSE);

    hwnd_ = CreateWindowW(kMainClass, L"Кредит на жилье", style, CW_USEDEFAULT, CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, this);
    if (!hwnd_) return false;
    ShowWindow(hwnd_, nCmdShow);
    UpdateWindow(hwnd_);
    return true;
}

HWND MainView::MakeStatic(const wchar_t* text, int x, int y, int w, int h, bool bold)
{
    HWND c = CreateWindowW(L"STATIC", text, WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, w, h,
                           hwnd_, nullptr, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(c, WM_SETFONT, (WPARAM)(bold && boldFont_ ? boldFont_ : GetStockObject(DEFAULT_GUI_FONT)), TRUE);
    return c;
}

void MainView::CreateControls()
{
    LOGFONTW lf = {};
    GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
    lf.lfWeight = FW_BOLD;
    boldFont_ = CreateFontIndirectW(&lf);

    const wchar_t* labels[7] = {
        L"Процентная ставка:", L"Сумма кредита:", L"Срок кредита:",
        L"Ежемесячный платёж:", L"Общая сумма выплат банку:",
        L"Переплата (сумма):", L"Переплата (во сколько раз):" };

    for (int i = 0; i < 7; ++i)
    {
        int y = 20 + i * 30 + (i >= 3 ? 20 : 0);   // разрыв между вводом и результатом
        MakeStatic(labels[i], 20, y, 230, 20);
        values_[i] = MakeStatic(L"—", 260, y, 190, 20, i >= 3);
    }

    HWND btn = CreateWindowW(L"BUTTON", L"Ввести данные", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                             20, 268, 430, 38, hwnd_, (HMENU)(INT_PTR)ID_BTN_ENTER,
                             GetModuleHandleW(nullptr), nullptr);
    SendMessageW(btn, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
}

void MainView::OnModelChanged()
{
    if (!model_.HasResult()) return;
    wchar_t buf[128];

    swprintf_s(buf, L"%.2f %%", model_.Rate());                SetWindowTextW(values_[0], buf);
    swprintf_s(buf, L"%.2f", model_.Amount());                 SetWindowTextW(values_[1], buf);
    swprintf_s(buf, L"%d лет (%d мес.)", model_.Years(), model_.Years() * 12);
                                                               SetWindowTextW(values_[2], buf);
    swprintf_s(buf, L"%.2f", model_.MonthlyPayment());         SetWindowTextW(values_[3], buf);
    swprintf_s(buf, L"%.2f", model_.TotalPayment());           SetWindowTextW(values_[4], buf);
    swprintf_s(buf, L"%.2f", model_.Overpayment());            SetWindowTextW(values_[5], buf);
    swprintf_s(buf, L"в %.2f раза", model_.OverpaymentRatio()); SetWindowTextW(values_[6], buf);
}

LRESULT CALLBACK MainView::WndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    MainView* self;
    if (m == WM_NCCREATE)
    {
        self = (MainView*)((CREATESTRUCTW*)l)->lpCreateParams;
        self->hwnd_ = h;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)self);
    }
    else
        self = (MainView*)GetWindowLongPtrW(h, GWLP_USERDATA);

    return self ? self->HandleMessage(m, w, l) : DefWindowProcW(h, m, w, l);
}

LRESULT MainView::HandleMessage(UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateControls();
        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_ENTER && HIWORD(wp) == BN_CLICKED && onEnter_)
            onEnter_();
        return 0;
    case WM_DESTROY:
        if (boldFont_) DeleteObject(boldFont_);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd_, msg, wp, lp);
}
