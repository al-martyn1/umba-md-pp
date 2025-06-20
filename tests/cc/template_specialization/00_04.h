// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<class T> // primary template
class X;
template<>        // specialization (declared, not defined)
class X<int>;
 
X<int>* p; // OK: pointer to incomplete type
X<int> x;  // error: object of incomplete type
