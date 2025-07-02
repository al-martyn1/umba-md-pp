// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<class T>
void f(T) { /* ... */ }
template<>
inline void f<>(int) { /* ... */ } // OK, inline
 
template<class T>
inline T g(T) { /* ... */ }
template<>
int g<>(int) { /* ... */ }         // OK, not inline
 
template<typename>
[[noreturn]] void h([[maybe_unused]] int i);
template<> void h<int>(int i)
{
    // [[noreturn]] has no effect, but [[maybe_unused]] has
}
