#ifndef UTILS__HEADER__FILE__
#define UTILS__HEADER__FILE__

#include<utility>
using namespace std;

template<class T>
pair<T,T> minmax(const T &a, const T &b) 
{
    return (a<b) ? pair<T,T>(a,b) : pair<T,T>(b,a);
}

template<class T>
void minmaxEx(T &a, T &b) 
{
    if (a<b)
    {
	T t=a; a=b; b=t;
    }
}

#endif // UTILS__HEADER__FILE__
