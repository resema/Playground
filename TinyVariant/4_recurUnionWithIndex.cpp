#include <iostream>

/* ***************************************************** */
// index
template<std::size_t I> using _index
 = std::integral_constant<std::size_t, I>;

/* ***************************************************** */
// Union_
template<bool TriviallyDestructible, typename ...Ts>
struct union_ {};
 
// trivial union
template <typename T, typename ...Ts>
struct union_<true, T, Ts...> {
    // base case cstor
    template<typename ...Args>
    explicit constexpr union_(_index<0>, Args&&... args)
    :   head(std::forward<Args>(args)...) {}

    // specialization cstor
    template<std::size_t I, typename ...Args>
    explicit constexpr union_(_index<I>, Args&&... args)
    :   tail(_index<I-1>{}, std::forward<Args>(args)...) {}

    // dstor
    ~union_() = default; // trivial
 
    T head;
    union_<true, Ts...> tail;
};
 
// non-trivial unions
template <typename T, typename ...Ts>
struct union_<false, T, Ts...> {
    // base case cstor
    template<typename ...Args>
    explicit constexpr union_(_index<0>, Args&&... args)
    :   head(std::forward<Args>(args)...) {}

    // specialization cstor
    template<std::size_t I, typename ...Args>
    explicit constexpr union_(_index<I>, Args&&... args)
    :   tail(_index<I-1>{}, std::forward<Args>(args)...) {}

    // dstor
    ~union_() {
        // user-provided, can't possibly know what to do
    }
 
    T head;
    union_<false, Ts...> tail;
};

/* ***************************************************** */
// Storage, handles the union
template <bool TriviallyDestructible, typename ...Ts>
struct storage;
 
template <typename ...Ts>
struct storage<true, Ts...> {
    // base case cstor
    constexpr storage() noexcept
    :   buffer(_index<0>{})
    ,   which(0)
    {}

    // specialization cstor
    template<std::size_t I, typename ...Args>
    explicit constexpr storage(_index<I> i, Args&&... args)
    :   buffer(i, std::forward<Args>(args)...){}
    ,   which(i)
    {}

    // dstor
    ~storage() = default; // still trivial
 
    union_<true, Ts...> buffer;
    std::size_t which;
};
 
template <typename ...Ts>
struct storage<false, Ts...> {
    // base case cstor
    constexpr storage() noexcept
    :   buffer(_index<0>{})
    ,   which(0)
    {}

    // specialization cstor
    template<std::size_t I, typename ...Args>
    explicit constexpr storage(_index<I> i, Args&&... args)
    :   buffer(i, std::forward<Args>(args)...){}
    ,   which(i)
    {}

    // dstor
    ~storage() {
        // dispatch to the appropriate destructor
    }
 
  union_<false, Ts...> buffer;
  std::size_t which;
};

/* ***************************************************** */
// helpers
template<bool...v> struct _all_of {
    static constexpr bool value = (v && ...);
};

/* ***************************************************** */
// Variant
template <typename ...Ts>
class variant {
    // 1-based index of T in Ts
    template<typename T> using _index_of = _index<I>;

    // trivially destructible
    static constexpr bool _trivially_destructible =
        _all_of<std::is_trivially_destructible_v<Ts>...>::value;
 
public:
    // `constexpr` constructor that is not a copy or move constructor
    constexpr variant() noexcept
    :   _storage{}
    {}

    template<typename T>
    constexpr variant(const T &v)
    :   _storage{_index_of<T>{}, v}
    {} 
 
  // trivial if all types in Ts are trivially destructible
  ~variant() = default;
 
  /*...*/
 
private:
  // literal type if all types in Ts are trivially destructible
  storage<_trivially_destructible, Ts...> _storage;
};

/* ***************************************************** */
// main
int main()
{
    return 0;
}