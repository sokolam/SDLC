#pragma once
#include <string>

// Сырые данные, введённые пользователем (строки как есть).
// Хранятся в главной форме и восстанавливаются при повторном вводе.
struct LoanInput
{
    std::wstring rate;    // процентная ставка, % годовых
    std::wstring amount;  // сумма кредита
    std::wstring years;   // срок, лет
};
