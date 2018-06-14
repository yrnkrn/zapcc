#pragma once
class Bug557_ScriptExecutionContext;
struct Bug557_Document;
class Bug557_Node {
  virtual void nodeName();
  Bug557_Document &document();
  virtual Bug557_ScriptExecutionContext *sc();
};
class Bug557_ScriptExecutionContext {};
class Bug557_HTMLElement;
struct Bug557_Document : Bug557_ScriptExecutionContext {
  void m_fn1(Bug557_HTMLElement);
};
inline Bug557_ScriptExecutionContext *Bug557_Node::sc() {
  return &document();
}
