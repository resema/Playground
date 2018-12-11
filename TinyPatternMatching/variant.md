## C++ Meeting 2018

### Pattern Matching mit std::variant



## what is a std::variant


### std::variant
- represents a type-safe union <!-- .element: class="fragment" -->
- holds a value of one of its types OR no value <!-- .element: class="fragment" -->
- it is not permitted to hold refs, arrays or void <!-- .element: class="fragment" -->


### handling
- default constructed with the value of its first alternative <!-- .element: class="fragment" -->
 - `std::monostate` for non default-constructible 
- throws <!-- .element: class="fragment" --> `std::bad_variant_access` <!-- .element: class="fragment" -->


### memory consumption
- additional memory for knowing its current state <!-- .element: class="fragment" -->
- but no dynamic allocation <!-- .element: class="fragment" -->


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


## problems with unions
- no way to know its content type <!-- .element: class="fragment" -->
- no lifetime support <!-- .element: class="fragment" -->



## how to access a std::variant


### std::visit
- visitor pattern to access a std::variant <!-- .element: class="fragment" -->
- visitor has to be a callable that accepts any type of the std::variant <!-- .element: class="fragment" -->

