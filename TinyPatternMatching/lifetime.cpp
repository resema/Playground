#include <variant>
#include <iostream>

class Widget
{
public:
    Widget() { std::cout << "Widget construction" << std::endl; }
    ~Widget() { std::cout << "Widget destruction" << std::endl; }
};

class OtherWidget
{
public:
    OtherWidget() { std::cout << "OtherWidget construction" << std::endl; }
    OtherWidget(const OtherWidget&) { std::cout << "OtherWidget copy-construction" << std::endl; }
    ~OtherWidget() { std::cout << "OtherWidget destruction" << std::endl; }
};

int main()
{
    //
    // Union
    std::cout << "Union\n-----" << std::endl;
    union Union
    {
        ~Union() {}
        Widget widget;
        OtherWidget otherWidget;
    };

    // Aggregate intialitation instead of writing a cstor
    Union u = { Widget() };

    //
    // Variant
    std::cout << "\nVariant\n-------" << std::endl;
    std::variant<Widget, OtherWidget> var;
    var = OtherWidget();
}