template <typename T> void Bug2034finalize(void *obj) { static_cast<T *>(obj)->~T(); }
void Bug2034foo(void(void *));
struct Bug2034LifecycleNotifier {
  virtual ~Bug2034LifecycleNotifier();
};
struct Bug2034WorkerThreadLifecycleContext final : virtual Bug2034LifecycleNotifier {
  __attribute__((annotate(""))) void *operator new(unsigned long) {
    Bug2034foo(Bug2034finalize<Bug2034WorkerThreadLifecycleContext>);
  }
  ~Bug2034WorkerThreadLifecycleContext();
};
