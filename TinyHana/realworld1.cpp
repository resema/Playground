//
// Real world examples from 'C++Now 2017, Louis Dionne'
//  Compile with 'wandbox.org'

//
// Compile time strings
#include <cassert>
#include <type_traits>
#include <boost/hana.hpp>
namespace hana = boost::hana;

template<char ...c> struct string {};

template<typename CharT, CharT ...c>
constexpr string<c...> operator"" _s() { return {}; }

template<char ...c1, char ...c2>
constexpr auto operator==(string<c1...>, string<c2...>) {
    return std::is_same<string<c1...>, string<c2...>> {};
}

template<char ...c1, char ...c2>
constexpr auto operator+(string<c1...>, string<c2...>) {
    return string<c1..., c2...> {};
}

//
// example 0 : Compile time string testing
auto hello_world = "hello"_s + " world"_s;
static_assert(hello_world == "hello world"_s);

int main()
{
}

//
// example 1
#include <boost/hana.hpp>
namespace hana = boost::hana;

auto Types = hana::tuple_t<int, void, char, long, void>;

auto NoVoid = hana::remove_if(Types, [](auto t) {
  return hana::traits::is_void(t);
});
// -> hana::tuple_t<int, char, long>

auto Ptrs = hana::transform(Types, [](auto t) {
  return hana::traits::add_pointer(t);
});
// -> hana::tuple_t<int*, void*, char*, long*, void*>

int main() {
}

//
// example 2 : A Parser
#include <cassert>
#include <sstream>
#include <boost/hana.hpp>
namespace hana = boost::hana;

// basic parser 
template<typename T>
struct parser
{
    T operator()(std::istream& in) const
    {
        T result;
        in >> result;
        return result;
    }
};
template<typename T>
parser<T> parse() { return {}; }

// literal parser
struct void_ {};
struct literal_parser
{
    char c;
    void_ operator()(std::istream& in) const
    {
        in.ignore(1, c);
        return {};
    }
};
literal_parser lit(char c) { return {c}; }

// combine parsers
template<typename ...Parsers>
auto combine_parsers(Parsers const& ...parsers) 
{
    return [=](std::istream& in) {
        hana::tuple<decltype(parsers(in))...> all{parsers(in)...};
        auto result = hana::remove_if(all, [](auto const& result) {
            return hana::typeid_(result) == hana::type<void_>{};
        });
        return result;
    };
}

int main()
{
    auto parser = combine_parsers(
        lit('('), parse<int>(),
        lit(','), parse<std::string>(),
        lit(','), parse<double>(),
        lit(')')
    );

    std::istringstream text{"(1 , foo , 3.3)"};
    hana::tuple<int, std::string, double> data = parser(text);
  
    hana::for_each(data, [&](const auto& x) {
        std::cout << x << std::endl;
    });
  
    assert(data == hana::make_tuple(1, "foo", 3.3));
}

//
// example 3 : A Simple Event System
template<typename ...Events>
event_system<Events...> make_event_system(Events ...events)
{
    return {};
}

int main()
{
    auto events = make_event_system(
        // "foo"_e = function<void(string)>,
        "bar"_e = function<void(int)>,
        "baz"_e = function<void(double)>
    );

    //
    // string needs another implementation to know them at compile time
    
    // events.on("foo"_e, [](string s) { 
    //     std::cout << "foo with '" << s << "'!\n"; 
    // });
    // events.on("foo"_e, [](string s) {
    //     std::cout << "foo with '" << s << "' again\n";
    // });
    events.on("bar"_e, [](int i) {
        std::cout << "bar with '" << i << "'!\n";
    });
    envents.on("baz"_e, [](double d) {
        std::cout << "baz with '" << d << "'!\n";
    });
    // events.on("unknown"_e, []...); // compiler error!

    // events.trigger("foo"_e, "hello"); // no overhead for event lookup
    events.trigger("bar"_e, 4);
    events.trigger("baz"_e, 3.3);
    // events.trigger("unknown"_e); // compiler error!
}

//
// example 4: Type erasure
#include <cassert>
#include <type_traits>
#include <boost/hana.hpp>
namespace hana = boost::hana;

template<char ...c> struct string {};

template<typename CharT, CharT ...c>
constexpr string<c...> operator"" _s() 
{ 
    return {}; 
}

template<char ...c1, char ...c2>
constexpr auto operator==(string<c1...>, string<c2...>) {
    return std::is_same<string<c1...>, string<c2...>> 
{};
}

template<char ...c1, char ...c2>
constexpr auto operator+(string<c1...>, string<c2...>) {
    return string<c1..., c2...> {};
}

//
struct HasArea : decltype(trait(
    "area"_s = std::function<double(self const&)>
)) 
{};

//
template<typename Signature>
constexpr hana::basic_type<Signature> function 
{};

struct self;

template<char ...c>
struct string {
    template<typename Signature>
    constexpr auto operator=(Signature sig) const {
        return hana::make_pair(*this, sig);
    }
};

struct Circle
{
    double x, y, radius;
};

template<>
auto impl<HasArea, Circle> = make_impl(
    "area"_s = [](Circle const& self) -> double {
        return 3.1415 * (self.radius * self.radius);
    }
);