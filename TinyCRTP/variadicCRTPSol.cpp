#include <iostream>
#include <set>
#include <type_traits>

template<typename Derived>
class ExtraFeature1
{
    public:
        void extraMethod1()
        {
            auto derived = static_cast<Derived&>(*this);
            std::cout << "extra feature 1\n";
            derived.basicMethod();
        }
};
template<typename Derived>
class ExtraFeature2
{
    public:
        void extraMethod2()
        {
            auto derived = static_cast<Derived&>(*this);
            std::cout << "extra feature 2\n";
            derived.basicMethod();
        }
};
template<typename Derived>
class ExtraFeature3
{
    public:
        void extraMethod3()
        {
            auto derived = static_cast<Derived&>(*this);
            std::cout << "extra feature 3\n";
            derived.basicMethod();
        }
};

//
// HasFeature
//  compile checking
template<typename Derived,
         template<typename> typename ToCheck,
         template<typename> typename Current,
         template<typename> typename... Features>
constexpr bool HasFeatureOld()
{
    if constexpr(std::is_same<ToCheck<Derived>, Current<Derived>>::value)
        return true;
    else if constexpr(sizeof...(Features) == 0)
        return false;
    else
        return HasFeatureOld<Derived, ToCheck, Features...>();
}

template<typename Derived,
         template<typename> typename ToCheck,
         template<typename> typename... Features>
constexpr bool HasFeature()
{
    return (std::is_same<ToCheck<Derived>, Features<Derived>>::value || ...);
}

template<typename Derived, template<typename> typename... Features>
struct ExtraFeatures : public Features<Derived>...
{
};

//
// JoinTwoExtraFeatures
template<typename...>
struct JoinTwoExtraFeatures;

template<typename Derived,
         template<typename> typename Feature,
         template<typename> typename... Features1,
         template<typename> typename... Features2>
struct JoinTwoExtraFeatures<ExtraFeatures<Derived, Features1...>,
                            ExtraFeatures<Derived, Feature, Features2...>>
{
    using type = typename 
                std::conditional<
                    HasFeature<Derived, Feature, Features1...>(),
                    typename JoinTwoExtraFeatures<
                        ExtraFeatures<Derived, Features1...>,
                        ExtraFeatures<Derived, Features2...>
                    >::type,
                    typename JoinTwoExtraFeatures<
                        ExtraFeatures<Derived, Features1..., Feature>,
                        ExtraFeatures<Derived, Features2...>
                    >::type
                >::type;
};                      

template<typename Derived,
         template<typename> typename... Features1>
struct JoinTwoExtraFeatures<ExtraFeatures<Derived, Features1...>,
                            ExtraFeatures<Derived>>
{
    using type = ExtraFeatures<Derived, Features1...>;
};                  

//
// JoinExtraFeatures - any number of features
template<typename...>
struct JoinExtraFeatures;

template<typename Derived,
         typename... Packs,
         template<typename> typename... Features1,
         template<typename> typename... Features2>
struct JoinExtraFeatures<ExtraFeatures<Derived, Features1...>,
                         ExtraFeatures<Derived, Features2...>,
                         Packs...>
{
    using type = typename 
        JoinExtraFeatures<
            typename JoinExtraFeatures<
                ExtraFeatures<Derived, Features1...>,
                ExtraFeatures<Derived, Features2...>
            >::type,
            Packs...
        >::type;
};

template<typename Derived,
         template<typename> typename... Features1,
         template<typename> typename... Features2>
struct JoinExtraFeatures<
            ExtraFeatures<Derived, Features1...>,
            ExtraFeatures<Derived, Features2...>>
{
    using type = typename 
        JoinTwoExtraFeatures<
            ExtraFeatures<Derived, Features1...>,
            ExtraFeatures<Derived, Features2...>
        >::type;
};

//
// ExtraFeaturesXXX
template<typename Derived>
using ExtraFeaturesA = ExtraFeatures<Derived, ExtraFeature1, ExtraFeature2>;
template<typename Derived>
using ExtraFeaturesB = ExtraFeatures<Derived, ExtraFeature2, ExtraFeature3>;
template<typename Derived>
using ExtraFeaturesC = ExtraFeatures<Derived, ExtraFeature1, ExtraFeature3>;

//
// Class X
template<template<typename> typename... Skills>
class X : public JoinExtraFeatures<Skills<X<Skills...>>...>::type
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
