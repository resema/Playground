#include <iostream>
#include "raii.h"

int main()
{
    ScopedResource<Widget> res;
    std::cout << *res.raw();

    ScopedResource<Widget, int> res2(4211);
    std::cout << *res2.raw();

    ScopedResource<Widget, int> res3(42.11);
    std::cout << *res3.raw();

    ScopedResource<Widget, double> res4(42.11);
    std::cout << *res4.raw();

    Widget* w = new Widget(42.11);
}