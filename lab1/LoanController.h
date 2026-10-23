#pragma once
#include "LoanModel.h"
#include "MainView.h"

// Контроллер (Controller): принимает действия пользователя от вида,
// разбирает ввод, обращается к модели. Результат вид получает сам —
// через уведомление модели (активная модель).
class LoanController
{
public:
    LoanController(LoanModel& model, MainView& view);

private:
    void OnEnterData();
    bool Submit(const LoanInput& in, std::wstring& error);

    LoanModel& model_;
    MainView& view_;
};
