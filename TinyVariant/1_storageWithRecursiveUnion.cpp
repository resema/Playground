#include <iostream>

template <typename ...Ts>
union storage {};
    
template <typename T, typename ...Ts>
union storage<T, Ts...> {
    T head;
    storage<Ts...> tail;
};

int main()
{
    // Look at it with 'cppinsights.io' to see the template creation
	storage<int, float> v1{42};

    // look at it with 'cppinsights.io' to see the deletion of the cstors
    storage<int, float> v2;

    // stoarage<int, std::string> v3; // does not compile
}