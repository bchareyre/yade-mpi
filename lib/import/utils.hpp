#pragma once

#include<utility>

template<class T>
std::pair<T,T> minmax(const T &a, const T &b) 
{
    return (a<b) ? std::pair<T,T>(a,b) : std::pair<T,T>(b,a);
}

template<class T>
void minmaxEx(T &a, T &b) 
{
    if (a<b)
    {
	T t=a; a=b; b=t;
    }
}

