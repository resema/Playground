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

struct X
{
};

int main() {
    auto f1 = std::function<void(int)>([](int){ std::cout << "int!\n"; });
    auto spX = std::make_unique<X>();

    std::variant<std::string, int> var;
    var = 42;
    std::visit(
        overload(
            f1, // copied
            [uX = std::move(spX)](const std::string&){ std::cout << "string!\n"; } // move only
        ), var
    );
}