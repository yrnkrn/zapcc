#pragma once
template <typename> void Bug1581d_AtomizeChars();
inline void Bug1581d_getRawTemplateStringAtom() { Bug1581d_AtomizeChars<int>(); }
