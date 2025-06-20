// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<typename T>
struct A
{
    struct B {};      // member class 
 
    template<class U> // member class template
    struct C {};
};
 
template<> // specialization
struct A<int> 
{
    void f(int); // member function of a specialization
};
// template<> not used for a member of a specialization
void A<int>::f(int) { /* ... */ }
 
template<> // specialization of a member class
struct A<char>::B
{
    void f();
};
// template<> not used for a member of a specialized member class either
void A<char>::B::f() { /* ... */ }
 
template<> // specialization of a member class template
template<class U>
struct A<char>::C
{
    void f();
};
 
// template<> is used when defining a member of an explicitly
// specialized member class template specialized as a class template
template<>
template<class U>
void A<char>::C<U>::f() { /* ... */ }

