#pragma once
#include <string>
#include <vector>

// Наблюдатель: вид подписывается на модель (АКТИВНАЯ модель MVC)
class IObserver
{
public:
    virtual ~IObserver() = default;
    virtual void OnModelChanged() = 0;
};

// Модель: данные и бизнес-логика расчёта аннуитетного кредита.
// Сама уведомляет подписчиков об изменении состояния.
class LoanModel
{
public:
    void AddObserver(IObserver* o);
    void RemoveObserver(IObserver* o);

    // Проверка + расчёт. При успехе уведомляет наблюдателей.
    // При ошибке возвращает false и текст ошибки, состояние не меняется.
    bool SetParameters(double ratePercent, double amount, int years, std::wstring& error);

    bool   HasResult() const      { return hasResult_; }
    double Rate() const           { return rate_; }
    double Amount() const         { return amount_; }
    int    Years() const          { return years_; }
    double MonthlyPayment() const { return monthly_; }
    double TotalPayment() const   { return total_; }
    double Overpayment() const    { return total_ - amount_; }
    double OverpaymentRatio() const { return amount_ > 0 ? total_ / amount_ : 0; }

private:
    void NotifyAll();

    std::vector<IObserver*> observers_;
    bool   hasResult_ = false;
    double rate_ = 0, amount_ = 0, monthly_ = 0, total_ = 0;
    int    years_ = 0;
};
