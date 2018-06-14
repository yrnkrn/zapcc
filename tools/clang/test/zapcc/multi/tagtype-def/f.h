#pragma once
template <typename> struct Bug612PersistentRootedMarker;
struct Bug612G;
struct Bug612G {
  friend Bug612PersistentRootedMarker<int>;
};
struct Bug612AutoTraceSession {
  Bug612G runtime;
};
