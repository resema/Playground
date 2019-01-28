#include <iostream>

//
// Union_
template<bool TriviallyDestructible, typename ...Ts>
union union_ {};
 
template <typename T, typename ...Ts>
union union_<true, T, Ts...> {
  ~union_() = default; // trivial
 
  T head;
  union_<true, Ts...> tail;
};
 
template <typename T, typename ...Ts>
union union_<false, T, Ts...> {
  ~union_() {
     // user-provided, can't possibly know what to do
  }
 
  T head;
  union_<false, Ts...> tail;
};

//
// Storage, handles the union
template <bool TriviallyDestructible, typename ...Ts>
struct storage;
 
template <typename ...Ts>
struct storage<true, Ts...> {
  ~storage() = default; // still trivial
 
  union_<true, Ts...> buffer;
  std::size_t which;
};
 
template <typename ...Ts>
struct storage<false, Ts...> {
  ~storage() {
    // dispatch to the appropriate destructor
  }
 
  union_<false, Ts...> buffer;
  std::size_t which;
};

//
// helpers
template<bool...v> struct _all_of {
    static constexpr bool value = (v && ...);
};

//
// Variant
template <typename ...Ts>
class variant {
  static constexpr bool _trivially_destructible =
      _all_of<std::is_trivially_destructible_v<Ts>...>::value;
 
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
  storage<_trivially_destructible, Ts...> _storage;
};


//
// main
int main()
{
	variant<int, float> v1;
}