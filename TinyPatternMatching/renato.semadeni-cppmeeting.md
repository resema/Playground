## C++ Meeting 2018
---
### Pattern Matching mit std::variant



## what is a variant


### CbVariant
- represents a type-safe union<!-- .element: class="fragment" -->
- provides a small interface<!-- .element: class="fragment" -->
 - <p> `IsNull`, `SetNull` </p> <!-- .element: class="fragment" -->
 - <p> `GetType` </p> <!-- .element: class="fragment" -->
 - <p> `ConvertTo`, `ConvertToLocalized` </p> <!-- .element: class="fragment" -->
 - <p> `Serialize` </p> <!-- .element: class="fragment" -->


### implementation of CbVariant
- <p> contains an `enum` to indicate containing element</p> <!-- .element: class="fragment" -->
- <p> data is hold by an `union` of all possible types </p> <!-- .element: class="fragment" -->
- works mainly with type conversion <!-- .element: class="fragment" -->
- doesn't know which types can be assigned <!-- .element: class="fragment" -->
- doesn't handle lifetime of its objects <!-- .element: class="fragment" -->


### std::variant
- represents a type-safe union <!-- .element: class="fragment" -->
- holds a value of one of its types OR no value <!-- .element: class="fragment" -->
- it is not permitted to hold refs, arrays or void <!-- .element: class="fragment" -->
- C++17 feature<!-- .element: class="fragment" -->


### tiny example
```cpp
std::variant<int, float> intFloat;
intFloat = 42;

assert(std::get<int> == 42);
```



## problems with unions
- no way to know its content type <!-- .element: class="fragment" -->
- no lifetime support <!-- .element: class="fragment" -->


### handling
- default constructed with the value of its first alternative <!-- .element: class="fragment" -->
 - <p>`std::monostate` for non default-constructible</p> <!-- .element: class="fragment" -->
- <p>throws `std::bad_variant_access`</p> <!-- .element: class="fragment" -->


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

```shell
$ ./default
0, value 0
```


### non default constructible types
```cpp
class NoDefaultCstor
{
    public:
        NoDefaultCstor(int, float) {}
};
```
```cpp
// compile-time error
std::variant<NoDefaultCstor, int> cannotInit; 
```

Note:
- [gcc](https://godbolt.org/z/wTZkCy)
- [clang](https://godbolt.org/z/zD_rsc)


### std::monostate
- unit type intended for use as a well-behaved empty alternative
- could be aliased to give a better name


### we have to use std::monostate
```cpp
class NoDefaultCstor
{
    public:
        NoDefaultCstor(int, float) {}
};
```
```cpp
std::variant<std::monostate, NoDefaultCstor, int> okInit;

assert(std::get_if<std::monostate>(&okInit));
```


### how to initialize?
``` cpp
std::variant<int, float> intFloat;

// by assignment operator
intFloat = 10;                                              

// by emplace<Idx>
intFloat.emplace<1>(10.3f);                                 

// by a visitor
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


### outcome
```cpp
std::variant<Widget, OtherWidget> var;
var = OtherWidget();
```
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
    var.emplace<1>(ThrowingWidget()); // var may be valueless
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



## std::visit
- most probably uses a N-dimensional matrix of function pointers <!-- .element: class="fragment" -->
- dispatches between those entries <!-- .element: class="fragment" -->
- two possible approaches described by M. Park (reference implementation)<!-- .element: class="fragment" -->
 - jump table <!-- .element: class="fragment" -->
 - recursive switch<!-- .element: class="fragment" -->
- C++17 feature <!-- .element: class="fragment" -->


### do we want a CbVisit
- does not exist <!-- .element: class="fragment" -->
- <p> current problems when realized for `CbVariant` </p> <!-- .element: class="fragment" -->
 - <p> `CbVariant` is not aware of its type possibilities nor indices </p> <!-- .element: class="fragment" -->
 - <p> could be realized by augmenting `CbVariant` to a template </p> <!-- .element: class="fragment" -->


 ### C++14 implementation of std::variant and std::visit
 - easy to achieve by using the reference implementation by M. Park <!-- .element: class="fragment" -->
 - <p> doesn't provide the benefits of `CbVariant` such as Serialization </p> <!-- .element: class="fragment" -->



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
      ProcessA,
      ProcessB,
      ...
);
```


### what can we get?
- multiple overloaded call-operator <!-- .element: class="fragment" -->
- generic lambda <!-- .element: class="fragment" -->

Note: 
- But how to get what we want? Matching lambdas.


### proposal to std::overload
- creates a class derived from multiple lambdas <!-- .element: class="fragment" -->
- can easily be implemented until its defined by the standard <!-- .element: class="fragment" -->
- probably C++20? <!-- .element: class="fragment" -->


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
auto f1 = std::function<void(int)>([](int){ /* ... */ });
auto spX = std::make_unique<X>();
```
```cpp
std::variant<std::string, int> var;

std::visit(
    overload(
        f1,
        [uX = std::move(spX)](std::string&){ /* ... */ }
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
      [](const std::pair<double, double> &arg){ /* ... */ },
      [](const double &arg){ /* ... */ },
      [](std::monostate){ /* ... */ }
      );
```

Note:
- also possible a std::function or a functor


### example from mattermost discussion
```cpp
struct CTestA {
    CTestA(int iVal) {}
};
struct CTestB {
    CTestB(float fVal) {}
};
```
```cpp
using TestCase = std::variant<CTestA,
                              CTestB>;
```


### execute the tests
```cpp
void distinguish(const TestCase &rTest) {
    match(rTest,
          [](const CTestA &rCaseA) { /* ... */ },
          [](const CTestB &rCaseB) { /* ... */ });
}
```
```cpp
std::vector<TestCase> vTests;
vTests.emplace_back(CTestA(42));
vTests.emplace_back(CTestA(11));
vTests.emplace_back(CTestB(42.11f));

for(const auto& rTest : vTests) {
    distinguish(rTest);
}
```


### benefits
```cpp
struct X {};

std::variant<std::string, X> var;
```
```cpp
match(var,
      [](int i){ /* ... */ },
      [](std::string &str) { /* ... */ });
```

Note:
- missing cases become compile-time errors
 - if not possible to be converted by a cast


 ### polymorphism
```cpp
struct Triangle {
    void Render() { /* ... */ }
};
struct Polygon {
    void Render() { /* ... */ }
};
```
```cpp
std::vector<std::variant<Triangle, Polygon>> objects{
    Polygon(),
    Triangle()
};
```
```cpp
auto CallRender = [](auto &obj) { obj.Render(); };

for(const auto &rObj : objects) {
    std::visit(CallRender, obj);
}
```

Note:
- Generic lambda for interface


### variant vs inheritance
<div class="left">
<p> closed to alternatives </p>
<p> open to new operations </p>
<p> single level </p>
<p> functional </p>
<p> simple </p>
</div>

<div class="right">
<p> open to alternatives </p>
<p> closed to new operations </p>
<p> multi-level </p>
<p> object oriented </p>
<p> can become complex </p>
</div>



## further interesting topics
- Ranges <!-- .element: class="fragment" -->
- Concepts <!-- .element: class="fragment" -->
- Time Travel Debugging <!-- .element: class="fragment" -->
- Design by Introspection (DbI) <!-- .element: class="fragment" -->
- Coroutines <!-- .element: class="fragment" -->


### ranges


### concepts


### time travel debugging
- new tool by Microsoft <!-- .element: class="fragment" -->
- additional to dump files <!-- .element: class="fragment" -->
- large trace files <!-- .element: class="fragment" -->
- allows stepping forward and backward in crashed code <!-- .element: class="fragment" -->
- can be downloaded from the Windows Store <!-- .element: class="fragment" -->
 - <p>`WinDbg Preview`</p> <!-- .element: class="fragment" -->


### design by introspection (DbI)
<p>*'The next big thing'* <small>Andrei Alexandrescu</small> </p><!-- .element: class="fragment" -->


### key words from DbI
- DbI Input <!-- .element: class="fragment" -->
 - Introspection types: "What are your methods?" <!-- .element: class="fragment" -->
 - Variant: "Do you support method xyz?" <!-- .element: class="fragment" -->
 - Strongest: "Does this code compile?" <!-- .element: class="fragment" -->
- DbI Processing <!-- .element: class="fragment" -->
 - Arbitrary compile-time evaluation <!-- .element: class="fragment" -->
- DbI Output<!-- .element: class="fragment" -->
 - Generate arbitrary code <!-- .element: class="fragment" -->


### tl;dr
 Questions?