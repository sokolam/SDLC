#pragma once
#include <windows.h>
#include <functional>
#include <string>
#include "LoanModel.h"
#include "LoanInput.h"

// Представление (View): главное окно. Подписано на модель (IObserver),
// перерисовывается при её изменении. Пользовательские действия передаёт
// контроллеру через обработчик.
class MainView : public IObserver
{
public:
    explicit MainView(LoanModel& model) : model_(model) {}

    bool Create(HINSTANCE hInst, int nCmdShow);
    HWND Handle() const { return hwnd_; }

    void SetEnterHandler(std::function<void()> h) { onEnter_ = std::move(h); }

    // Последние введённые данные (хранятся в главной форме)
    const LoanInput& LastInput() const { return lastInput_; }
    void SetLastInput(const LoanInput& in) { lastInput_ = in; }

    // IObserver
    void OnModelChanged() override;

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT msg, WPARAM wp, LPARAM lp);
    void CreateControls();
    HWND MakeStatic(const wchar_t* text, int x, int y, int w, int h, bool bold = false);

    LoanModel& model_;
    HWND hwnd_ = nullptr;
    HWND values_[7] = {};   // ставка, сумма, срок, платёж/мес, всего, переплата, во сколько раз
    HFONT boldFont_ = nullptr;
    std::function<void()> onEnter_;
    LoanInput lastInput_;
};
