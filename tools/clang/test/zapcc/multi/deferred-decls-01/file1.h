#pragma once
class Bug557ScriptExecutionContext {};
class Bug557HTMLElement;
class Bug557Document : public Bug557ScriptExecutionContext {
  void m_fn1(Bug557HTMLElement);
};
inline Bug557ScriptExecutionContext *Bug557Node::sc() {
  return &Bug557document();
}
