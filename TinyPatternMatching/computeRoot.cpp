#include <variant>
#include <iostream>
#include <cmath>

template<class... Ts> struct overload : Ts...
{
    using Ts::operator()...;
};
template<class... Ts> overload(Ts...) -> overload<Ts...>;

using VarRoot = std::variant<std::pair<double, double>, double, std::monostate>;

VarRoot computeRoots(double a, double b, double c)
{
    double discriminant = b*b - 4*a*c;
    if(discriminant > 0.0)
    {
        double p = sqrt(discriminant) / (2*a);
        return std::make_pair(-b + p, -b - p);
    }
    else if(discriminant == 0.0)
    {
        return (-1*b) / (2*a);
    }
    else
    {
        return std::monostate();
    }
}

void printResult(const VarRoot &var)
{
    std::visit(overload {
        [](const std::pair<double, double>& arg){

        }
    })
}

int main()