#pragma once
  
struct VirtualDestructorBase {
  virtual ~VirtualDestructorBase();
};

struct VirtualDestructorDerived : VirtualDestructorBase {
  VirtualDestructorDerived() {}
  virtual void anchor();
};

static void VirtualDestructorCreate() { new VirtualDestructorDerived(); }
