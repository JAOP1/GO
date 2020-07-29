#pragma once
#include <iostream>
#include <vector>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& array)
{
    os<<"vector:"<<std::endl;
    for(T v : array)
        os<<v<<" ";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<T>>& Matrix)
{
    os<<"******Matrix******"<<std::endl;
    for(std::vector<T>& vect : Matrix)
        os<< vect << std::endl;
    return os;
}


