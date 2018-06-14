#pragma once
namespace Bug905fc {
template <typename> struct optional {
  template <typename> friend struct optional;
};
template <typename a> struct fwd { void foo(); };
template <typename b> b addressof;
}
