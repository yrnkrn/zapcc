#pragma once

template <typename IPAddrType>
void FS_templatedGlobalFunc(IPAddrType ip) {
  ip.something();
}

struct FS_IPAddress {
  void wrapperFunc() {
    FS_templatedGlobalFunc(*this);
  }
  void something();
};
