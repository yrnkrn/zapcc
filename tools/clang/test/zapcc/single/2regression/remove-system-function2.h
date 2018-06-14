struct IOBuf {
  IOBuf();
};

template <typename T>
void make_unique() { new IOBuf; }

void clone() {
  make_unique<IOBuf>();
}
