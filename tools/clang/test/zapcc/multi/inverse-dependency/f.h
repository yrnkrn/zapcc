#pragma once
namespace Bug329NS {
template <int> struct Bug329TemplateStruct {
  template <int> void foo() {}
};
}
