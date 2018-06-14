#pragma once
template <typename> class Bug1844ValueMapCallbackVH {};
template <typename KeyT> void Bug1844foo() {
  Bug1844DenseMapInfo<Bug1844ValueMapCallbackVH<KeyT>>::getEmptyKey();
}
template <typename KeyT> struct Bug1844DenseMapInfo<Bug1844ValueMapCallbackVH<KeyT>> {
  static void getEmptyKey() {}
};
