template <typename> struct enable_shared_from_this {};
template <typename T> struct shared_ptr {
  shared_ptr(enable_shared_from_this<T> *p) { (void)static_cast<T *>(p); }
};
class abstract_read_executor;
shared_ptr<abstract_read_executor> foo();
