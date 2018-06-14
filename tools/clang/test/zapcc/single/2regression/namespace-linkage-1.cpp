// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 826
extern "C" char Bug826environ;
namespace {
extern "C" char Bug826environ;
}
