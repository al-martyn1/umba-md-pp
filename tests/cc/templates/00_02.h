// https://en.cppreference.com/w/cpp/language/template_specialization.html

namespace N
{
    template<class T> // primary template
    class X { /*...*/ };
    template<>        // specialization in same namespace
    class X<int> { /*...*/ };
 
    template<class T> // primary template
    class Y { /*...*/ };
    template<>        // forward declare specialization for double
    class Y<double>;
}
 
template<> // OK: specialization in same namespace
class N::Y<double> { /*...*/ };

