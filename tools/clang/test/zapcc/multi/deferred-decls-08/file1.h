#pragma once
template <typename> struct Bug897smart_ref { Bug897smart_ref(const Bug897smart_ref &); };
struct Bug897fee_schedule;
struct Bug897c {
  Bug897smart_ref<Bug897fee_schedule> a;
  Bug897c() : a(a) {}
};
