#include <variant>
#include <iostream>

int main()
{
    std::cout << "sizeof string: :" << sizeof(std::string) << std::endl;

    std::cout << "sizeof variant<std::string>: " << sizeof(std::variant<std::string>) << std::endl;

    std::cout << "sizeof variant<int, std::string>: " << sizeof(std::variant<int, std::string>) << std::endl;

    std::cout << "sizeof variant<int, float>: " << sizeof(std::variant<int, float>) << std::endl;
}