#include <variant>
#include <iostream>

// overloaded lambdas
template<class... Ts> struct overload : Ts...
{
    using Ts::operator()...;
};
template<class... Ts> overload(Ts...) -> overload<Ts...>;

int main()
{
    std::variant<int,float> intFloat;
    intFloat = 1.5f;
    
    std::visit(overload{
        [](int &i) { i *= 2; },
        [](float &f) {f *= 2.0f; }
    }, intFloat);

    auto PrintVisitor = [](const auto& t){ std::cout << t << std::endl; };
    std::visit(PrintVisitor, intFloat);
}