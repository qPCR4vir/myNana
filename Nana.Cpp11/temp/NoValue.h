#ifndef _NO_VALUE_H
#define _NO_VALUE_H

#include "C:\Prog\ExtLib\std\limits_constexpr.h"

constexpr int i = 2;
constexpr int ixi(int i_){ return i_ * i_; }
constexpr int j= { STD::numeric_limits<int>::max() };


template <typename Num, Num NoValue = i+i /*= STD::numeric_limit<Num>::max()*/>
struct WithNoValue
{
    Num _i;


};
WithNoValue<int> I;

#endif