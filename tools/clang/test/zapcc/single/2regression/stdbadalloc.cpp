// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 724
namespace std {
class bad_alloc {};
}
std::bad_alloc bad;
