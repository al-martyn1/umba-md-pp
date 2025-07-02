// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<class T>
class Array { /*...*/ };
 
template<class T> // primary template
void sort(Array<T>& v);
template<>        // specialization for T = int
void sort(Array<int>&);
 
// no need to write
// template<> void sort<int>(Array<int>&);
