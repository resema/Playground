//
// Real world examples from 'C++Now 2017, Louis Dionne'
//  Compile with 'wandbox.org'

//
// example 1:
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
// example 2;
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