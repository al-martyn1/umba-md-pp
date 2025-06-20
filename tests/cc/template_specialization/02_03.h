// https://en.cppreference.com/w/cpp/language/template_specialization.html

template<class T1>
class A
{
    template<class T2>
    class B
    {
        template<class T3> // member template
        void mf1(T3);
 
        void mf2();        // non-template member
    };
};
 
// specialization
template<>        // for the specialized A
template<class X> // for the unspecialized B
class A<int>::B
{
    template<class T>
    void mf1(T);
};
 
// specialization
template<>        // for the specialized A
template<>        // for the specialized B
template<class T> // for the unspecialized mf1
void A<int>::B<double>::mf1(T t) {}
 
// ERROR: B<double> is specialized and is a member template, so its enclosing A
// must be specialized also
template<class Y>
template<>
void A<Y>::B<double>::mf2() {}
