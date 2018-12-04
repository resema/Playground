#include <variant>
#include <iostream>

template<class T1, class T2>
struct overload2 : T1, T2
{
    overload2(const T1& t1, const T2& t2) : T1(t1), T2(t2)
    {
    }

    using T1::operator();
    using T2::operator();
};

int main()
{
    std::variant<std::string, int> var;
    var = 1;

    std::visit(
        overload2(
            [](int){ std::cout << "int!\n"; },
            [](const std::string&){ std::cout << "string!\n"; }
        ),
        var
    );
}