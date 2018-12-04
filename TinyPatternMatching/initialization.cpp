#include <variant>
#include <iostream>

class NotSimpleConstructible
{
    public:
        NotSimpleConstructible(int, float) {}
};

int main()
{
    {
        // default initialization
        std::variant<int, float> intFloat;
        std::cout << intFloat.index() << ", value " << std::get<int>(intFloat) << std::endl;
    }
    {
        // std::variant<NotSimpleConstructible, int> cannotInit; // compile-time error
        std::variant<std::monostate, NotSimpleConstructible, int> okInit;
        std::cout << okInit.index() << std::endl;
    }
    {
        // pass a arbituary value
        std::variant<int, float> intFloat(10.4f);
        // std::variant<int, float>intFloat(10.444); // double can be castet into `int` or `float` -> ambiguity
        std::cout << intFloat.index() << ", value " << std::get<float>(intFloat) << std::endl;
    }
    {
        // solve ambiguity by `in_place_index`
        std::variant<int, float>intFloat(std::in_place_index<0>, 10.4444);
        std::cout << intFloat.index() << ", value " << std::get<int>(intFloat) << std::endl;
    }
    {
        // Initialization possibilities
        std::variant<int, float> intFloat;
        intFloat = 10;                                              // Assignment operator
        intFloat.emplace<1>(10.3f);                                 // `emplace`
        std::get<float>(intFloat) += 1.0f;                          // `std::get` returns a reference
        if (auto pFloat = std::get_if<float>(&intFloat); pFloat)    // Visitor
        {
            *pFloat *= 2.0f;
        }
        std::cout << intFloat.index() << ", value " << std::get<float>(intFloat) << std::endl;
    }
}