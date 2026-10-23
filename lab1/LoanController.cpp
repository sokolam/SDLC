#include "LoanController.h"
#include "InputDialog.h"
#include <cmath>
#include <cstdlib>

namespace
{
    std::wstring Trim(const std::wstring& s)
    {
        size_t b = s.find_first_not_of(L" \t\r\n");
        if (b == std::wstring::npos) return L"";
        size_t e = s.find_last_not_of(L" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    bool ParseDouble(const std::wstring& text, double& out)
    {
        std::wstring s = Trim(text);
        if (s.empty()) return false;
        for (auto& ch : s) if (ch == L',') ch = L'.';
        wchar_t* end = nullptr;
        out = wcstod(s.c_str(), &end);
        return end && *end == L'\0' && std::isfinite(out);
    }

    bool ParseInt(const std::wstring& text, int& out)
    {
        std::wstring s = Trim(text);
        if (s.empty()) return false;
        wchar_t* end = nullptr;
        long v = wcstol(s.c_str(), &end, 10);
        if (!end || *end != L'\0') return false;
        out = (int)v;
        return true;
    }
}

LoanController::LoanController(LoanModel& model, MainView& view)
    : model_(model), view_(view)
{
    view_.SetEnterHandler([this] { OnEnterData(); });
}

void LoanController::OnEnterData()
{
    LoanInput input = view_.LastInput();   // восстановить последние данные
    InputDialog::Show(view_.Handle(), input,
        [this](const LoanInput& in, std::wstring& err)
        {
            view_.SetLastInput(in);        // запомнить в главной форме (даже если ввод неверный)
            return Submit(in, err);
        });
}

bool LoanController::Submit(const LoanInput& in, std::wstring& error)
{
    double rate = 0, amount = 0;
    int years = 0;
    if (!ParseDouble(in.rate, rate))   { error = L"Процентная ставка должна быть числом."; return false; }
    if (!ParseDouble(in.amount, amount)) { error = L"Сумма кредита должна быть числом."; return false; }
    if (!ParseInt(in.years, years))    { error = L"Срок кредита должен быть целым числом лет."; return false; }
    return model_.SetParameters(rate, amount, years, error);
}
