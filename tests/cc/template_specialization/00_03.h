// https://en.cppreference.com/w/cpp/language/template_specialization.html

class String {};
 
template<class T>
class Array { /*...*/ };
 
template<class T> // primary template
void sort(Array<T>& v) { /*...*/ }
 
void f(Array<String>& v)
{
    sort(v); // implicitly instantiates sort(Array<String>&), 
}            // using the primary template for sort()
 
template<> // ERROR: explicit specialization of sort(Array<String>)
void sort<String>(Array<String>& v); // after implicit instantiation
