#include "future.hh"
template <typename> struct Bug2508shared_future {
  Bug2508shared_future() { Bug2508future<>().get_available_state(); }
};
void shared_promise() { Bug2508shared_future<int>(); }
