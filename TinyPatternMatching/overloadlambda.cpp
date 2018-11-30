#include <variant>
#include <iostream>

template<class ...Ts>
struct overload : Ts...
{
    // Type deduction works because the cstor is NOT a template
    // BUT it copies the lambdas into the base object
    overload(const Ts&... ts) : Ts(ts)...
    {
    }

    using Ts::operator()...;
};

struct X
{
};

int main()
{
    auto f1 = std::function<void(int)>([](int){ std::cout << "int!\n"; });
    auto spX = std::make_unique<X>();

    std::variant<std::string, int> var;
    std::visit(
        overload(
            [](int){ std::cout << "int!\n"; },
            [](const std::string&){ std::cout << "string!\n"; }
        ),
        var
    );
}