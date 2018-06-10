#include "typerich.h"

void applyMomentumToBody(const Momentum& impulseValue)
{
}

int main()
{
    const double someVal = 13.75;
    // applyMomentumToBody(someVal);    // compile-time error

    Force force{13.75};
    // applyMomentumToBody(force);         // compile-time error

    Momentum momentum{13.75};
    applyMomentumToBody(momentum);

    constexpr Momentum impulseValue = Force{30.0} * Time{3.0};
    applyMomentumToBody(impulseValue);

    constexpr Force combinedValue = 15.0_N + 10.0_N;

    constexpr Momentum impulseValue2 = 30.0_N * 3.0_s;
    applyMomentumToBody(impulseValue2);
}