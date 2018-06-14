#pragma once
struct Bug830GLUI_Control {
  void c(char *d) { __builtin_memcpy(d, d, 1); }
  virtual void mouse_down_handler();
  virtual ~Bug830GLUI_Control();
};
struct Bug830GLUI_Mouse_Interaction : Bug830GLUI_Control {
  void mouse_down_handler();
};
