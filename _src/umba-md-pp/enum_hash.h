#pragma once

#include <unordered_set>


// https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        typedef typename std::underlying_type<T>::type  EnumUnderlyingType;
        return std::hash<EnumUnderlyingType>{}(static_cast<EnumUnderlyingType>(t));
    }
};