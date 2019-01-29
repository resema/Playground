#include <iostream>
#include <assert.h>
#include <functional>

// helper structs
struct _empty{};
template<typename T>
struct _identity
{
  	using type = T;
};
struct _fallback{};
struct _ambiguous{};

using _fallback_fun_ptr = _fallback(*)(...);

// Index
template<std::size_t I>
struct _index {
  	static constexpr std::size_t value = I;
};

// Pack
template <typename ...Ts>
struct pack
{
    using type = pack;
	static constexpr std::size_t size = sizeof...(Ts);
};

template <typename T, T ...Vs>
struct pack_c
{
	using type = pack_c;
	static constexpr std::size_t size = sizeof...(Vs);
};

// index helper
template <typename Is, bool Odd>
struct _make_index_pack_twice;

template <std::size_t ...Is>
struct _make_index_pack_twice<
  	pack_c<std::size_t, Is...>
  	, false
    > 
:	pack_c<std::size_t, Is..., (sizeof...(Is) + Is)...>
{};

template <std::size_t ...Is>
struct _make_index_pack_twice<
  	pack_c<std::size_t, Is...>
    , true
  	> 
:	pack_c<std::size_t, Is..., (sizeof...(Is) + Is)..., sizeof...(Is) * 2>
{};

template <std::size_t N>
struct _make_index_pack
: 	_make_index_pack_twice<
  		typename _make_index_pack<N / 2>::type
        , N % 2 != 0
        >
{};

template <>
struct _make_index_pack<1>
: 	pack_c<std::size_t, 0>
{};

template <>
struct _make_index_pack<0>
: 	pack_c<std::size_t>
{};

template <typename Ts>
struct _index_pack;

template <typename ...Ts>
struct _index_pack<pack<Ts...>>
: 	_make_index_pack<sizeof...(Ts)>
{};

template <typename Ts>
using index_pack_t = typename _index_pack<Ts>::type;

template<std::size_t I, typename T>
struct _indexed {};

template<typename Ts, typename Is = index_pack_t<Ts>>
struct _indexer;

template<typename ...Ts, std::size_t ...Is>
struct _indexer<pack<Ts...>, pack_c<std::size_t, Is...>>
:	_indexed<Is, Ts>...
{};

// at index
template<std::size_t I>
static _empty _at_index(...);

template<std::size_t I, typename T>
static _identity<T> _at_index(_indexed<I, T> const&);

template<std::size_t I, typename Ts>
struct at_index
:	decltype(_at_index<I>(_indexer<Ts>{}))
{};

// invoke
template <typename F, typename T>
static auto _invoke(int)
-> decltype(std::declval<F>()(std::declval<T>()));

template <typename F, typename T>
static _ambiguous _invoke(...);

// Overload
template <std::size_t I, typename T>
struct _overload
{
  using fun_ptr = _index<I>(*)(T);
  operator fun_ptr();
};

template <typename Ts, typename Is = index_pack_t<Ts>>
struct _overloads;

template <typename ...Ts, std::size_t ...Is>
struct _overloads<pack<Ts...>, pack_c<std::size_t, Is...>>
: _overload<Is, Ts>...
{
	operator _fallback_fun_ptr();
};

// Result of
template<typename F, typename T,
		 typename R = decltype(_invoke<F,T>(0))>
struct result_of : R
{
	static constexpr bool viable = true;
};

template<typename F, typename U>
struct result_of<F, U, _fallback>
{
	static constexpr bool viable = false;
};

template<typename U, typename Ts>
struct index_of_best_match
:	result_of<_overloads<Ts>, U>
{};

// _union
template<typename Ts>
struct _union;

template<>
struct _union<pack<>>
{};

template<typename T, typename ...Ts>
struct _union<pack<T, Ts...>>
{
	static constexpr std::size_t size = 1 + sizeof...(Ts);
    
    template<typename ...Args>
    constexpr _union(_index<0>, Args&&... args)
    :	_head(std::forward<Args>(args)...)
    {}
    
    template<std::size_t I, typename ...Args>
    constexpr _union(_index<I>, Args&&... args)
    :	_tail(_index<I - 1>{}, std::forward<Args>(args)...)
    {}
    
    private:
      union
      {
          T _head;
          _union<pack<Ts...>> _tail;
      };
};

// storage
template<typename Ts>
struct _storage;

template<typename ...Ts>
struct _storage<pack<Ts...>>
:	_union<pack<Ts...>>
{
	using base_type = _union<pack<Ts...>>;
    
    // cstor
	constexpr _storage()
    :	base_type{_index<0>{}}
    ,	_which{0}
    {}
    // cstor with argument
    template<std::size_t I, typename ...Args>
    constexpr _storage(_index<I> which, Args&&... args)
    :	base_type{which, std::forward<Args>(args)...}
    ,	_which{I}
    {}
    
    protected:
	    int _which;
};

template<typename ...Ts>
using storage = _storage<pack<_empty, Ts...>>;

// variant
template<typename ...Ts>
class variant {
	public:
  		// cstor
        constexpr variant()
        :	_storage{}
        {}
        // cstor with argument
        template<typename U,
        		 std::size_t I = index_of_best_match<
                 	U&&, pack<Ts...>>::value,
                 typename T = typename at_index<
                 	I, pack<Ts...>>::type
                 >
        constexpr variant(U&& v)
        :	_storage{_index<I + 1>{}, std::forward<U>(v)}
        {}
        
  	private:
		storage<Ts...> _storage;  
};

int main()
{
	variant<int, float> intFloat{42.11f};
}


