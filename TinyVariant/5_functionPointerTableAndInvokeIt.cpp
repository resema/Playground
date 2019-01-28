#include <iostream>
#include <assert.h>
#include <functional>

// helper structs
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

	///////////////////////////////////////////////////////////////////////////
    template <typename Is, bool Odd>
    struct _make_index_pack_twice;

    template <std::size_t ...Is>
    struct _make_index_pack_twice<
        pack_c<std::size_t, Is...>
      , false
    > : pack_c<std::size_t, Is..., (sizeof...(Is) + Is)...>
    {};

    template <std::size_t ...Is>
    struct _make_index_pack_twice<
        pack_c<std::size_t, Is...>
      , true
    > : pack_c<std::size_t, Is..., (sizeof...(Is) + Is)..., sizeof...(Is) * 2>
    {};

    template <std::size_t N>
    struct _make_index_pack
      : _make_index_pack_twice<
            typename _make_index_pack<N / 2>::type
          , N % 2 != 0
        >
    {};

    template <>
    struct _make_index_pack<1>
      : pack_c<std::size_t, 0>
    {};

    template <>
    struct _make_index_pack<0>
      : pack_c<std::size_t>
    {};

    template <typename Ts>
    struct _index_pack;

    template <typename ...Ts>
    struct _index_pack<pack<Ts...>>
      : _make_index_pack<sizeof...(Ts)>
    {};

    template <typename Ts>
    using index_pack_t = typename _index_pack<Ts>::type;
    ///////////////////////////////////////////////////////////////////////////

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

int main()
{
  	std::size_t val = index_of_best_match<float, pack<int, float>>::value;
	printf("%i", val);
}

