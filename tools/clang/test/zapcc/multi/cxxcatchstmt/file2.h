#pragma once
struct Bug380ExclusiveFilter {
  Bug380ExclusiveFilter() {
    try {
    } catch (Bug380shared_ptr<int>) {
    }
  }
};
