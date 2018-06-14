#pragma once
namespace Bug327NS {
template <class T> void Bug327Func() { (void)(T() + 0); }
}
using Bug327NS::Bug327Func;
