#include "file1.h"
#include "addressof.hpp"
namespace Bug905fc {
template <typename a> void fwd<a>::foo() {}
void tcp_socket::readsome() { my.foo(); }
}
