#pragma once
template <typename Functor> void Bug1834manage() { new Functor; }
struct Bug1834BOOST_FUNCTION_VTABLE {
  void (*manager)();
};
template <typename Functor> void Bug1834function(Functor) {
  Bug1834BOOST_FUNCTION_VTABLE stored_vtable = {Bug1834manage<Functor>};
}
