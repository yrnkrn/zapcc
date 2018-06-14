#pragma once
template <bool> struct Bug1090c { static constexpr bool value = false; };
template <bool b> constexpr bool Bug1090c<b>::value;
template <bool> struct Bug1090conditional {};
