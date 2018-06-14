#pragma once
template <class, class> struct Bug1783is_same {};
template <class NodeAlgorithms> struct Bug1783generic_hook {
  Bug1783is_same<NodeAlgorithms, Bug1783dft_tag> a;
};
