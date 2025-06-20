// https://en.cppreference.com/w/cpp/language/template_specialization.html

#include <type_traits>
 
template<typename T> // primary template
struct is_void : std::false_type {};
template<>           // explicit specialization for T = void
struct is_void<void> : std::true_type {};
 
int main()
{
    static_assert(is_void<char>::value == false,
        "for any type T other than void, the class is derived from false_type");
    static_assert(is_void<void>::value == true,
        "but when T is void, the class is derived from true_type");
}
