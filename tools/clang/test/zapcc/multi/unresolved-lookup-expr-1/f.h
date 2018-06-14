#pragma once
namespace UnresolvedLookupExprNS1 {
struct S {};
}
void operator+(int, UnresolvedLookupExprNS1::S);
namespace UnresolvedLookupExprNS2 {
template <typename T> void bar() { 0 + T(); }
}
