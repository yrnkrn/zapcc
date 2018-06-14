#pragma once
template <typename Target, typename Source> Target &Bug427downcast(Source &source) {
  (Target &)(source);
  (void)dynamic_cast<Target &>(source);
  (void)reinterpret_cast<Target &>(source);
  (void)static_cast<Target &>(source);
}
struct Bug427ScriptExecutionContext;
struct Bug427Document;
