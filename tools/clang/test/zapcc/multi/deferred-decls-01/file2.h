#pragma once
class Bug557ScriptExecutionContext;
class Bug557Document;
Bug557Document &Bug557document();
class Bug557Node {
  virtual void nodeName();
  virtual Bug557ScriptExecutionContext *sc();
};
