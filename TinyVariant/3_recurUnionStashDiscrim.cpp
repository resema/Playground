#include <iostream>

// storage
template <typename T>
struct indexed {
  std::size_t which;
  T member;
};
 
template <std::size_t I, typename ...Ts>
union storage {};
 
template <std::size_t I, typename T, typename ...Ts>
union storage<I, T, Ts...> {
  static_assert(std::is_standard_layout<T>::value, "");
 
  indexed<T> head;
  storage<I - 1, Ts...> tail;
};


// Variant
template <typename ...Ts>
class variant {
  static constexpr int _size_of = sizeof...(Ts);
 
public:
  // `constexpr` constructor that is not a copy or move constructor
  constexpr variant() noexcept
    : _storage{}
  {}
 
  // trivial if all types in Ts are trivially destructible
  ~variant() = default;
 
  /*...*/
 
private:
  // literal type if all types in Ts are trivially destructible
  storage<_size_of, Ts...> _storage;
};

int main()
{
	variant<int, float> v1;
}