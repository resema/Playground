#include <variant>
#include <iostream>

struct SWidget1 
{
};
struct SWidget2 
{
};

struct SVisitor
{
    std::ostream& rOStream;

    SVisitor(std::ostream &rOS) 
    : rOStream(rOS)
    {
    }

    void operator()(const SWidget1 &rW1)
    {
        rOStream << "It's an Oner!" << std::endl;
    } 

    void operator()(const SWidget2 &rW2)
    {
        rOStream << "It's an Twoer!" << std::endl;
    } 
};

int main()
{
    using SumType = std::variant<SWidget1, SWidget2>;

    SumType vWidget = SWidget1();
    std::visit(SVisitor{std::cout}, vWidget);
}