// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 299
// ClassFriendItself<int> Lexical DeclContext is ClassFriendItself<C>!
#include "class-friend-itself.h"
class C {};
boost::ClassFriendItself<C> c;
#include "class-friend-itself-typedef.h"
int main() {
}
