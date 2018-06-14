#pragma once
template <typename> class UserClassTemplatedFriend {
  template <typename T>
  friend bool operator<(const UserClassTemplatedFriend<T> &,
                        const UserClassTemplatedFriend<T> &);
};
