// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_SERVER_H
#define ZAPCC_SERVER_H

#include "llvm/Config/llvm-config.h"

class Server {
  Server(Server &) = delete;
  Server &operator=(Server &) = delete;

#ifdef LLVM_ON_UNIX
  int ServerSocketFdInOut;
  int ServerSocketFdError;
#endif
#ifdef LLVM_ON_WIN32
  void *PipeInputHandle;
  void *PipeOutputHandle;
  void *PipeErrorHandle;
#endif
  int ServerId;

public:
  Server(int Id, int FdInOut, int FdErr);
  ~Server();
  void done();
  bool connect();
  bool disconnect();
  bool writeExitCode(int Retcode);
};

#endif // ZAPCC_SERVER_H
