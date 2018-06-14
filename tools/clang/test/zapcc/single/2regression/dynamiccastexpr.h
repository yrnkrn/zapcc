#pragma once
template <typename> struct Process { virtual ~Process() {} };
template <typename T> void dispatch(Process<T> *process) {
  (void)dynamic_cast<T *>(process);
}
class LogStorageProcess;
