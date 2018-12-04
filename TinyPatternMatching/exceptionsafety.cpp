#include <variant>
#include <iostream>

class ThrowingClass
{
public:
    explicit ThrowingClass(int i)
    {
        if(i == 0) 
        {
            throw std::exception();
        }
    }
};

int main()
{
    std::variant<int, ThrowingClass> var;
    var = 11;

    // change the value, old state still valid -> no assignment happend
    try
    {
        var = ThrowingClass(0);
    }
    catch (...)
    {
        std::cout << "catch(...)" << std::endl;

        // we keep the old state!
        std::cout << var.valueless_by_exception() << std::endl;
        std::cout << std::get<int>(var) << std::endl;
    }

    // inside `emplace`, this is different
    var = ThrowingClass(10);
    try
    {
        var.emplace<1>(ThrowingClass(0));
    }
    catch(...)
    {
        std::cout << "catch(...)" << std::endl;

        // the old state was already destroyed, not valid state
        std::cout << var.valueless_by_exception() << std::endl;
        std::cout << std::get<int>(var) << std::endl;
    }
}