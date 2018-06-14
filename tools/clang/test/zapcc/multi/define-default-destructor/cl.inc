struct Bug1819cl {
  ~Bug1819cl() = default;
  Bug1819unique_ptr<Bug1819cl *> goo;
};
