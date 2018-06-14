#pragma once
namespace boost {
template <class T> class ClassFriendItself {
  template <class Y> friend class ClassFriendItself;
};
}
