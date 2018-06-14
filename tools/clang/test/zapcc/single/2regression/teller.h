#ifndef TELLER_H
#define TELLER_H

class teller {
  const char* name;
public:
  teller(const char* _name) : name(_name) {
    printf("CTR::%s\n",name);
  }
  ~teller() {
    printf("DTR::%s\n",name);
  }
};

template <class T>T* plus1(T* x) { return x+2; }

template<class T>
class mynameis {
  const T* name;
public:
  mynameis(const T* _name) : name(_name) {
    printf("MYNAMEIS CTR::::%s\n",name);
  }
  ~mynameis() {
    printf("MYNAMEIS DTR::::%s\n",name);
  }
};

#endif // TELLER_H