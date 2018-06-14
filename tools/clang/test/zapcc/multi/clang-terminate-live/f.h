#pragma once
void Bug385Fn();
struct Bug385Struct {
  Bug385Struct() noexcept { Bug385Fn(); }
};
