#include <variant>
#include <iostream>
#include <cmath>

template<class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
    std::visit(overloaded{
        [](const std::pair<double, double>& arg){
            std::cout << "Two roots found: " << arg.first << ", " << arg.second << std::endl;
        },
        [](double arg){
            std::cout << "One root found: " << arg << std::endl;
        },
        [](std::monostate){
            std::cout << "No real root found." << std::endl;
        }
    }, var);
}

int main()
{
    printResult(computeRoots(1, -2, -2));
    printResult(computeRoots(1, 6, 9));
    printResult(computeRoots(1, -3, 4));
}