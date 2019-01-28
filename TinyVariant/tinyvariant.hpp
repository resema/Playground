#include <iostream>

// helpers indexpack
 

// helpers function pointer table
template<std::size_t I, typename T>
struct _overload
{
  using fun_ptr = index<I>(*)(T);
  operator fun_ptr();
};

// storage
template <size_t I, typename T>
struct indexed {
  using _index = std::integral_constant<std::size_t, I>;
  T _member;
};
 
template <std::size_t I, typename ...Ts>
struct storage {};
 
template <std::size_t I, typename T, typename ...Ts>
struct storage<I, T, Ts...> {
 	// default cstor
    storage() = default;
    // cstor
    template<typename Arg>
    storage(Arg&& arg)
    {
    	// need the information about which index the arg has...
    	head._member = arg;
    }
    
  	indexed<I, T> head;
  	storage<I + 1, Ts...> tail;
};


// Variant
template <typename ...Ts>
class variant {
  	static constexpr int _size_of = sizeof...(Ts);
 
public:
  	// `constexpr` constructor that is not a copy or move constructor
  	constexpr variant() noexcept
    : 	_storage{}
  	{}
  
  	template<typename Arg>
  	constexpr variant(Arg&& arg)
  	:	_storage(std::forward<Arg>(arg))		
  {}
  
 
  // trivial if all types in Ts are trivially destructible
  ~variant() = default;
 
  /*...*/
 
private:
  // literal type if all types in Ts are trivially destructible
  storage<1, Ts...> _storage;
};

int main()
{
	variant<int, float> v1{42.11f};
}