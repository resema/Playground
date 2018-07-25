#include <iostream>

template<typename T, template<typename> typename crtpType>
struct crtp
{
    T& underlying() { return static_cast<T&>(*this); }
    const T& underlying() const { return static_cast<const T&>(*this); }
};

template<typename T>
struct Square : public crtp<T, Square>
{
    void square()
    {
        this->underlying().setValue(this->underlying().getValue() * this->underlying().getValue());
    }
};

template<typename T>
struct Scale : public crtp<T, Scale>
{
    void scale(double value)
    {
        this->underlying().setValue(this->underlying().getValue() * value);
    }
};

template<template<typename> typename... Skills>
class Widget : public Skills<Widget<Skills...>>...
{
public:
    Widget() : magnitude(42) {}
    Widget(double val) : magnitude(val) {}

    double getValue() const { return magnitude; }
    void setValue(double value) { magnitude = value; }

    template<template<typename> typename... T>
    friend std::ostream& operator<<(std::ostream& outstream, const Widget<T...>& w);

private:
    double magnitude;
};

template<template<typename> typename... T>
std::ostream& operator<<(std::ostream& outstream, const Widget<T...>& w)
{
    outstream << "Widget value = " << w.magnitude;
    return outstream;
}

int main()
{
    using W1 = Widget<Square>;
    using W12 = Widget<Square, Scale>;

    W1 w1;
    w1.square();
    std::cout << w1 << std::endl;

    W12 w2(11);
    w2.square();
    w2.scale(10);
    std::cout << w2 << std::endl;
}