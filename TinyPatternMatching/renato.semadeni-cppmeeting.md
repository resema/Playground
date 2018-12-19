## C++ Meeting 2018

### Pattern Matching mit std::variant



## what is a std::variant


### std::variant
- represents a type-safe union <!-- .element: class="fragment" -->
- holds a value of one of its types OR no value <!-- .element: class="fragment" -->
- it is not permitted to hold refs, arrays or void <!-- .element: class="fragment" -->


## problems with unions
- no way to know its content type <!-- .element: class="fragment" -->
- no lifetime support <!-- .element: class="fragment" -->


### handling
- default constructed with the value of its first alternative <!-- .element: class="fragment" -->
 - `std::monostate` for non default-constructible 
- throws <!-- .element: class="fragment" --> `std::bad_variant_access` <!-- .element: class="fragment" -->


### memory consumption
- additional memory for knowing its current state <!-- .element: class="fragment" -->
- but no dynamic allocation <!-- .element: class="fragment" -->


### sizeof
```cpp
std::cout <<  sizeof(std::string);                      // 24

std::cout << sizeof(std::variant<std::string>);         // 32

std::cout << sizeof(std::variant<int, std::string>);    // 32

std::cout << sizeof(std::variant<int, float>);          // 8
```


### default constructible types
```cpp
// default initialization
std::variant<int, float> intFloat;
std::cout << intFloat.index() << ", value " 
          << std::get<int>(intFloat) << std::endl;
```


### non default constructible types
```cpp
class NoDefCstor
{
    public:
        cstor(int, float) {}
};
```
```cpp
// compile-time error
std::variant<NoDefCstor, int> cannotInit; 
```


### we have to use std::monostate
```cpp
class NoDefCstor
{
    public:
        cstor(int, float) {}
};
```

```cpp
std::variant<std::monostate, NoDefCstor, int> okInit;
std::cout << okInit.index() << std::endl;
if (auto obj = std::get_if<std::monostate>(&okInit); obj)
{
    std::cout << "containg type is std::monostate\n";
}
```


### how to initialize?
``` cpp
std::variant<int, float> intFloat;
// Assignment operator
intFloat = 10;                                              
// emplace<Idx>
intFloat.emplace<1>(10.3f);                                 
// Visitor
if (auto pFloat = std::get_if<float>(&intFloat); pFloat)    
{
    *pFloat *= 2.0f;
}
```  


### lifetime
```cpp
class Widget
{
public:
    Widget() { std::cout << "Widget construction"; }
    ~Widget() { std::cout << "Widget destruction"; }
};
```
```cpp
class OtherWidget
{
public:
    OtherWidget() { std::cout << "Other construction"; }
    OtherWidget(const OtherWidget&) { 
        std::cout << "Other copy-construction"; 
    }
    ~OtherWidget() { std::cout << "Other destruction"; }
};
```
```cpp
std::variant<Widget, OtherWidget> var;
var = OtherWidget();
```


### outcome
``` shell
$ ./lifetime
Widget construction             # default init
OtherWidget construction        # cstor
Widget destruction              # lifetime ended
OtherWidget copy-construction   # assignment
OtherWidget destruction         # used for assignment
OtherWidget destruction         # end of scope
```


### what if I screw it up?
```cpp
struct ThrowingWidget {
    operator int() { throw 42; }
};
```

``` cpp
std::variant<float, int> var = 12.f; // OK
try {
    var.emplace<1>(S()); // var may be valueless
} catch (...) {
    std::cout << var.valueless_by_exception() << std::endl;
}
```

Note:
Do not use. Example is not self-speaking. 
Keep in mind, that ```valueless_by_exception``` can be used in such cases.


## how to access a std::variant?
```cpp
std::variant<int, float> intFloat;
// Accessing savely the variant
try
{
    auto f = std::get<float>(intFloat);
    std::cout << "float!" << std::endl;
}
catch (std::bad_variant_access&)
{
    // ...
}
```


### shorter version using `std::get_if`
```cpp
std::variant<int, float> intFloat;
if (const auto intPtr = std::get_if<0>(&intFloat))
{
    std::cout << "int!" << std::endl;
}
```

Note:
- interestingly `get_if` takes a pointer, `get` a reference...
- `nullptr` check possible through that.


### visitors
- visitor pattern to access a std::variant <!-- .element: class="fragment" -->
- visitor has to be a callable that accepts any type of the std::variant <!-- .element: class="fragment" -->


### process what you visit
```cpp
std::variant<int, float> intFloat;
// generic lambda 
auto PrintVisitor = [](const auto& t){ // ... };

std::visit(PrintVisitor, intFloat);
```

Note:
- works ONLY if types share the same interface!


### different actions based on active type
```cpp
struct MultiplyVisitor {
    void operator()(int &i) const {
        i *= static_cast<int>(fFactor);
    }

    void operator()(float &f) const {
        f *= fFactor;
    }
};
```
```cpp
std::variant<int, float> intFloat;

std::visit(MultiplyVisitor(0.5f), intFloat);
```



## pattern matching


### functional paradigm
- state machines <!-- .element: class="fragment" -->
- multiple return values (computation, error codes, ...) <!-- .element: class="fragment" -->
- parsing <!-- .element: class="fragment" -->
- event handling <!-- .element: class="fragment" -->
- polymorphism <!-- .element: class="fragment" -->


### how to match?


### what we want!
```cpp
match(variable,
    [](WidgetA a){
        // case A
    },
    [](WidgetB b){
        // case B
    });
```


### what can we get?
- multiple overloaded call-operator <!-- .element: class="fragment" -->
- generic lambda <!-- .element: class="fragment" -->

Note: 
- But how to get what we want? Matching lambdas.


### proposal to std::overload
- creates a class derived from multiple lambdas <!-- .element: class="fragment" -->
- can easily be implemented until its defined by the standard <!-- .element: class="fragment" -->


### overloading two lambdas
```cpp
template<class T1, class T2>
struct overload2 : T1, T2
{
    overload2(const T1& t1, const T2& t2) : T1(t1), T2(t2) {}

    using T1::operator();
    using T2::operator();
};
```
```cpp
std::variant<std::string, int> var;

std::visit(
    overload2(
        [](int){ std::cout << "int!\n"; },
        [](const std::string&){ std::cout << "string!\n"; }
    ), var);
```


### using variadic templates
```cpp
template<class ...Ts>
struct overload : Ts...
{
    overload(const Ts&... ts) : Ts(ts)... {}

    using Ts::operator()...;
};
```
```cpp
std::variant<std::string, int> var;
std::visit(
    overload(
        [](int){ std::cout << "int!\n"; },
        [](const std::string&){ std::cout << "string!\n"; }
    ), var);
```

Note:
- Type deduction works because the cstor is NOT a template
- BUT it copies the lambdas into the base object


### nitty-gritty details
```cpp
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

// cstor a template -> automatic deduction guides not working
template<class ...Us>
overload(Us&&...) -> overload<std::remove_reference_t<Us>...>;
```


### no copies anymore
```cpp
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
```


### let's create a match() method
```cpp
template <typename Variant, typename... Matchers>
auto match(Variant& variant, Matchers&&... matchers)
{
  return std::visit(
    overload{std::forward<Matchers>(matchers)...},
             variant);
}
```


### and we finally get
```cpp
match(variable,
    [](WidgetA a){
        // case A
    },
    [](WidgetB b){
        // case B
    });
```

&#10154; what we wanted!



## use cases


### different computational results
```cpp
using VarRoot = std::variant<std::pair<double, double>, 
                             double, 
                             std::monostate>;

VarRoot computeRoots(double a, double b, double c) {
    double discriminant = b*b - 4*a*c;
    if(discriminant > 0.0) {
        double p = sqrt(discriminant) / (2*a);
        return std::make_pair(-b + p, -b - p);
    }
    else if(discriminant == 0.0) {
        return (-1*b) / (2*a);
    }
    else { return std::monostate(); }
}
```


### let's match
```cpp
match(computeRoots(1, -2, -2),
      [](const std::pair<double, double> &arg){ // ... },
      [](const double &arg){ // ... },
      [](std::monostate){ // ... }
      );
```

Note:
- also possible a std::function or a functor


### example from mattermost discussion


### polymorphism


### benefits
Note:
- missing cases become compile-time errors
 - if not possible to be converted by a cast