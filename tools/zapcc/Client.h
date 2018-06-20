// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_CLIENT_H
#define ZAPCC_CLIENT_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Config/llvm-config.h"

enum class ScanCLIResult { Success, Error, CommandsRequest, ResetRequest };

#ifdef LLVM_ON_UNIX
typedef int ConnectionType;
#endif
#ifdef LLVM_ON_WIN32
typedef void *ConnectionType;
#endif

class Client {
public:
  typedef struct {
    ConnectionType InputHandle;
    ConnectionType OutputHandle;
    ConnectionType ErrorHandle;
  } ConnectionHandles;
  ConnectionHandles Connection;
  static const int NotFoundExitCode = -9999;
  enum class ConnectResult { Success, NotFound, Busy, Unexpected };
  Client(Client &) = delete;
  Client &operator=(Client &) = delete;
  Client(const char *ServerName, int ServerId);
  ~Client();
  bool connect();
  bool send(const std::string &Command);

private:
  const char *ServerName;
  int ServerId;
  const char *CName;
  const int MaxServers = 256;
#ifdef LLVM_ON_UNIX
  llvm::SmallString<1024> Name;
  int ServerSocketFdInOut;
  int ServerSocketFdError;
  int LockSocket;
#endif
#ifdef LLVM_ON_WIN32
  llvm::SmallString<1024> NameInput;
  llvm::SmallString<1024> NameOutput;
  llvm::SmallString<1024> NameError;
  const char *CNameInput;
  const char *CNameError;
  const char *CNameOutput;
#endif
  void setName();
  ConnectResult connectToOne(bool UseSpecificServer);
  bool connectToAny();
  ConnectResult waitAndOpenConnection(unsigned ms);
  ConnectResult open();
  ConnectResult createPipeAndServer();
  bool startServer();

protected:
  ConnectResult obtainLock();
  void releaseLock();
};

// Return 1 on error.
int startServerAndSend(llvm::ArrayRef<const char *> Argv, const char *Argv0);

#endif // ZAPCC_CLIENT_H
