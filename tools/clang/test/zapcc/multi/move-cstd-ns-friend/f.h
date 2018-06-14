#pragma once
namespace Bug1494m {
template <typename> class LockStats {
  template <typename> friend class LockStats;
};
}
