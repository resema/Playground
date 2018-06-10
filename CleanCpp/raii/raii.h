#ifndef RAII_H
#define RAII_H

#include <ostream>

template<typename RT, typename... Ts>
class ScopedResource final 
{
public:
    ScopedResource(Ts&&... params) { managedRes = new RT(std::forward<Ts>(params)...); }
    ~ScopedResource() { delete managedRes; }

    ScopedResource(const ScopedResource& rhs) = delete;
    ScopedResource& operator=(const ScopedResource& rhs) = delete;

    RT* operator->() const { return managedRes; }
    RT* raw() const { return managedRes; }

private:
    RT* managedRes;
};

class Widget
{
public:
    Widget() : myInt(42) {};
    Widget(int val) : myInt(val) {};
    ~Widget() = default;

    Widget(const Widget& rhs) = delete;
    Widget& operator=(const Widget& rhs) = delete;

private:
    friend std::ostream& operator<<(
        std::ostream& outstream, const Widget& widget
        );

private:
    int myInt;
};

std::ostream& operator<<(std::ostream& outstream, const Widget& widget)
{
    outstream << "Widget value = " << widget.myInt << std::endl;
    return outstream;
}

#endif  // RAII_H