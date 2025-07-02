// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<typename T>
struct A
{
    void f(T);         // member, declared in the primary template
 
    void h(T) {}       // member, defined in the primary template
 
    template<class X1> // member template
    void g1(T, X1);
 
    template<class X2> // member template
    void g2(T, X2);
};
 
// specialization of a member
template<>
void A<int>::f(int);
 
// member specialization OK even if defined in-class
template<>
void A<int>::h(int) {}
 
// out of class member template definition
template<class T>
template<class X1>
void A<T>::g1(T, X1) {}
 
// member template specialization
template<>
template<class X1>
void A<int>::g1(int, X1);
 
// member template specialization
template<>
template<>
void A<int>::g2<char>(int, char); // for X2 = char
 
// same, using template argument deduction (X1 = char)
template<> 
template<>
void A<int>::g1(int, char);
