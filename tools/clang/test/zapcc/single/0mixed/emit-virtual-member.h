#pragma once
void evm_IsEmpty();
struct evm_ByteQueue {
  virtual ~evm_ByteQueue();
  virtual void AnyRetrievable() { evm_IsEmpty(); }
};
