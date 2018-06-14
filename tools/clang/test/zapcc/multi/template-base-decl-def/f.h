#pragma once
template <int> class Bug675Quadrature;
template <int dim> void Bug675m_fn1(Bug675Triangulation, Bug675Quadrature<dim - 1>);
template <int dim> class Bug675Quadrature { Bug675Quadrature<dim - 1> SubQuadrature; };
template <> class Bug675Quadrature<0> {};
template <int dim> class Bug675QGauss : Bug675Quadrature<dim> {};
