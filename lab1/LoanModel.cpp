#include "LoanModel.h"
#include <algorithm>
#include <cmath>

void LoanModel::AddObserver(IObserver* o)
{
    if (o) observers_.push_back(o);
}

void LoanModel::RemoveObserver(IObserver* o)
{
    observers_.erase(std::remove(observers_.begin(), observers_.end(), o), observers_.end());
}

void LoanModel::NotifyAll()
{
    for (auto* o : observers_) o->OnModelChanged();
}

bool LoanModel::SetParameters(double ratePercent, double amount, int years, std::wstring& error)
{
    if (!std::isfinite(ratePercent) || ratePercent < 0 || ratePercent > 100)
    {
        error = L"Процентная ставка должна быть в диапазоне от 0 до 100 %.";
        return false;
    }
    if (!std::isfinite(amount) || amount <= 0 || amount > 1e12)
    {
        error = L"Сумма кредита должна быть положительным числом (не более 1 000 000 000 000).";
        return false;
    }
    if (years < 1 || years > 50)
    {
        error = L"Срок кредита должен быть целым числом лет от 1 до 50.";
        return false;
    }

    const int    n = years * 12;          // число платежей
    const double r = ratePercent / 1200.0; // месячная ставка

    // Аннуитетный платёж: P = S * r / (1 - (1 + r)^-n); при r = 0: P = S / n
    double payment = (r == 0.0) ? amount / n
                                : amount * r / (1.0 - std::pow(1.0 + r, -n));

    rate_ = ratePercent;
    amount_ = amount;
    years_ = years;
    monthly_ = payment;
    total_ = payment * n;
    hasResult_ = true;

    NotifyAll();
    return true;
}
