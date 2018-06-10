#ifndef TYPERICH_H
#define TYPERICH_H

template<int M, int K, int S>
struct MksUnit {
    enum class SI{meter = M, kilogram = K, second = S};
};

template<typename MksUnit>
class Value {
    private:
        long double magnitude{0.0};

    public:
        constexpr explicit Value(const long double magnitude)
            : magnitude(magnitude) {}
        constexpr long double getMagnitude() const { return magnitude; }
};

using Length        = Value<MksUnit<1,0,0>>;
using Area          = Value<MksUnit<2,0,0>>;
using Volume        = Value<MksUnit<3,0,0>>;
using Mass          = Value<MksUnit<0,1,0>>;
using Time          = Value<MksUnit<0,0,1>>;
using Acceleration  = Value<MksUnit<1,0,-2>>;
using Force         = Value<MksUnit<1,1,-2>>;
using Momentum      = Value<MksUnit<1,1,-1>>;
using Pressure      = Value<MksUnit<-1,1,-2>>;
// etc...

template<int M, int K, int S>
constexpr decltype(auto) operator+(
    const Value<MksUnit<M,K,S>>& lhs, const Value<MksUnit<M,K,S>>& rhs
    ) noexcept
{
    return Value<MksUnit<M,K,S>>(lhs.getMagnitude() + rhs.getMagnitude());
}

template<int M1, int K1, int S1, int M2, int K2, int S2>
constexpr decltype(auto)operator*(
    const Value<MksUnit<M1,K1,S1>>& lhs, const Value<MksUnit<M2,K2,S2>>& rhs
    ) noexcept 
{
    return Value<MksUnit<M1+M2,K1+K2,S1+S2>>(lhs.getMagnitude() * rhs.getMagnitude());
}

constexpr Force operator"" _N(long double magnitude) {
    return Force(magnitude);
}

constexpr Time operator"" _s(long double magnitude) {
    return Time(magnitude);
}

constexpr Momentum operator"" _Ns(long double magnitude) {
    return Momentum(magnitude);
}

#endif  // TYPERICH_H