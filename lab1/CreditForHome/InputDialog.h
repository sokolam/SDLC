#pragma once
#include <windows.h>
#include <functional>
#include <string>
#include "LoanInput.h"

// Дополнительное (модальное) окно для ввода данных.
class InputDialog
{
public:
    // Валидатор вызывается по нажатию «ОК». Если вернул false — окно остаётся
    // открытым, пользователю показывается текст ошибки.
    using Validator = std::function<bool(const LoanInput&, std::wstring& error)>;

    // Возвращает true, если данные приняты (ОК + валидация пройдена).
    static bool Show(HWND owner, LoanInput& data, Validator validator);
};
