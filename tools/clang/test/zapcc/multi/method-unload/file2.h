#pragma once
template <class> struct Bug928Disposable { void foo(); };
template <class a> void Bug928Disposable<a>::foo() { *this; }
