#pragma once
struct Bug917ad;
template <typename aj> struct Bug917ak {
  template <typename al, typename = decltype(al())> static Bug917ad am;
  typedef decltype(am<aj>) an;
};
template <typename b> void Bug917t(b) { using c = typename Bug917ak<b>::an; }
