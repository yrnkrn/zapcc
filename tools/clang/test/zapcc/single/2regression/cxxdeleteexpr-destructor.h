#pragma once
template <class _Tp> void Bug1435unique_ptr() { delete (_Tp *)nullptr; }
class Bug1435MCSubtargetInfo;
