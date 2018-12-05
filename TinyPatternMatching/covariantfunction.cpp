#include <variant>
#include <iostream>

template<class ...Ts>
struct overload : Ts...
{
    // cstor templated to allow only-movables
    template<class ...Us>
    overload(Us&& ...us) : Ts(std::forward<Us>(us))...
    {
    }

    using Ts::operator()...;
};

// cstor a template -> automatic deduction guides not working anymore.
template<class ...Us>
overload(Us&&...) -> overload<std::remove_reference_t<Us>...>;

int main()
{
    std::variant<int, double> var;
    var = 42.11;

    // not valid visitor, because return type is not common.
    // auto d = std::visit([](auto n){ return n + 1; }, var);

    // manual prediction of the possible states of the result
    auto next = [](auto n) -> std::variant<int, double> { return n + 1; };
    auto d = std::visit(next, var);
    assert(std::get<double>(d) == 42.11 + 1.0);

    var = 42;
    // doesn't scale well for complicated cases
    auto next_overload = overload(
        [](int a) -> std::variant<int, double> { return a + 1; },
        [](double a) -> std::variant<int, double> { return a + 1.1; }
    );
    auto res0 = std::visit(next_overload, var);
    assert(std::get<int>(res0) == (int)42.11 + 1);

    var = 42.11;
    auto res1 = std::visit(next_overload, var);
    assert(std::get<double>(res1) == 42.11 + 1.1);
}