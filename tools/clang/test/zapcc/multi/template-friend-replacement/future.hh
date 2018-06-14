#pragma once
template <typename...> class Bug2508future {
  void get_available_state();
  template <typename> friend class Bug2508shared_future;
};
template <typename T> Bug2508future<T> Bug2508make_ready_future();
