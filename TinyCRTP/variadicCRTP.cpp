#include <iostream>
#include <set>
#include <type_traits>

// base case 
template<typename, typename = std::void_t<>>
struct IsInPackageT 
    : std::false_type {};

// partial specialization
template<typename T>
struct IsInPackageT<T, 
                    std::void_t<decltype(std::declval<T>().isCreated())>> 
    : std::true_type {};

template<typename Derived, bool>
class ExtraFeature1
{
public:
    bool isCreated() { return true; }
    void extraMethod1()
    {
        auto derived = static_cast<Derived&>(*this);
        std::cout << "extra feature 1\n";
        derived.basicMethod();
    }
};
template<typename Derived>
class ExtraFeature1<Derived, true>
{
};

template<typename Derived, bool>
class ExtraFeature2
{
public:
    bool isCreated() { return true; }
    void extraMethod2()
    {
        auto derived = static_cast<Derived&>(*this);
        std::cout << "extra feature 2\n";
        derived.basicMethod();
    }
};
template<typename Derived>
class ExtraFeature2<Derived, true>
{
};

template<typename Derived, bool>
class ExtraFeature3
{
public:
    bool isCreated() { return true; }
    void extraMethod3()
    {
        auto derived = static_cast<Derived&>(*this);
        std::cout << "extra feature 3\n";
        derived.basicMethod();
    }
};

template<typename Derived>
struct ExtraFeaturesA : ExtraFeature1<Derived, IsInPackageT<Derived>::value>, 
                        ExtraFeature2<Derived, IsInPackageT<Derived>::value> {};

template<typename Derived>
struct ExtraFeaturesB : ExtraFeature2<Derived, IsInPackageT<Derived>::value>, 
                        ExtraFeature3<Derived, IsInPackageT<Derived>::value> {};

template<template<typename Feature> typename... Skills>
class X : public Skills<X<Skills...>>...
{
public:
    void basicMethod(){};
};

int main()
{
    using XAB = X<ExtraFeaturesA, ExtraFeaturesB>;
    
    XAB x;
    x.extraMethod1();
    x.extraMethod2();
    x.extraMethod3();
}
