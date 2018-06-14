#pragma once
template <typename, typename = void> struct Bug1735parse_impl;
template <typename Expr> struct Bug1735parse_impl<Expr> {
  template <typename Iterator> static void call(Iterator &, Expr const &) {}
};
