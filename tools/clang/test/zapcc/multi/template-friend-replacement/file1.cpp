#include "future.hh"
struct stop_iteration_tag;
Bug2508future<> foo1;
void do_for_each() { Bug2508make_ready_future<stop_iteration_tag>(); }
