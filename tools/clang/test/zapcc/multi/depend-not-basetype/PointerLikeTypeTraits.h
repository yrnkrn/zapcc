#pragma once
namespace {
template <int a> struct Bug906b : Bug906std2::integral_constant<int, Bug906b<a / 2>::c> {};
template <> struct Bug906b<1> : Bug906std2::integral_constant<int, 0> {};
}
