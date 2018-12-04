#include <variant>
#include <iostream>
#include <vector>

struct Triangle 
{
    void Render() { std::cout << "Drawing a triangle!" << std::endl; }
};

struct Polygon
{
    void Render() { std::cout << "Drawing a polygon!" << std::endl; }
};

struct Sphere
{
    void Render() { std::cout << "Drawing a sphere!" << std::endl; }
};

int main()
{
    // Initializer list initialization
    std::vector<std::variant<Triangle, Polygon, Sphere>> objects{
        Polygon(),
        Triangle(),
        Sphere(),
        Triangle()
    };

    // generic lambda
    auto CallRender = [](auto& obj) { obj.Render(); };

    for(auto &obj : objects)
    {
        std::visit(CallRender, obj);
    }

}