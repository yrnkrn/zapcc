#pragma once
template <int, int = 1> struct TemplateAfterFriend1 {};
template <class> struct TemplateAfterFriend1Helper {
  template <int, int> friend struct TemplateAfterFriend1;
};
