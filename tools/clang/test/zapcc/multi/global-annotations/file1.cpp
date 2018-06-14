#include "WorkerThread.h"
Bug2034WorkerThreadLifecycleContext::~Bug2034WorkerThreadLifecycleContext() {
  new Bug2034WorkerThreadLifecycleContext;
}
