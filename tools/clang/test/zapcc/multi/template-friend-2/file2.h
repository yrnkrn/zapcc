#pragma once
template <int dim = 1> struct TemplateAfterFriend2 {};
template <int> struct TemplateAfterFriendFoo2 {
  template <int> friend struct TemplateAfterFriend2;
};
