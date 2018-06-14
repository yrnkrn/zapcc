// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "Server.h"
#include "DisableStreamsBuffering.h"

#include "llvm/Support/raw_ostream.h"

#include <stdio.h>

#ifdef LLVM_ON_UNIX
#include "../zapcc/BasePort.h"
#include "StringConst.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif // LLVM_ON_UNIX

#ifdef LLVM_ON_WIN32
#include <iostream>
#include <windows.h>
#endif // LLVM_ON_WIN32

Server::Server(int Id, int FdInOut, int FdErr) : ServerId(Id) {
#ifdef LLVM_ON_UNIX
  ServerSocketFdInOut = FdInOut;
  ServerSocketFdError = FdErr;
#endif // LLVM_ON_UNIX
#ifdef LLVM_ON_WIN32
  // FIXME: can we always obtain GetStdHandle even when serverid <=0 (e.g.
  // zapccs @commands.txt)
  if (Id > 0) {
    PipeInputHandle = GetStdHandle(STD_INPUT_HANDLE);
    PipeOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    PipeErrorHandle = GetStdHandle(STD_ERROR_HANDLE);
  } else {
    PipeInputHandle = nullptr;
    PipeOutputHandle = nullptr;
    PipeErrorHandle = nullptr;
  }
#endif
} // LLVM_ON_WIN32

Server::~Server() { done(); }

bool Server::writeExitCode(int RetCode) {
  return putc(RetCode, stdout) != EOF;
}

#ifdef LLVM_ON_UNIX
bool Server::connect() {
  assert(ServerSocketFdInOut != -1 && ServerSocketFdError != -1);

  int ClientSocketOut = accept(ServerSocketFdInOut, NULL, NULL);
  if (ClientSocketOut < 0) {
    perror("connect: accept");
    return false;
  }
  // Redirect input/out/error streams to the socket.
  dup2(ClientSocketOut, STDIN_FILENO);
  dup2(ClientSocketOut, STDOUT_FILENO);
  // Close socket file descriptor if not stdio.
  if (ClientSocketOut > STDOUT_FILENO) {
    close(ClientSocketOut);
  }

  int ClientSocketError = accept(ServerSocketFdError, NULL, NULL);
  if (ClientSocketError < 0) {
    perror("connect: accept");
    return false;
  }
  dup2(ClientSocketError, STDERR_FILENO);
  if (ClientSocketError > STDERR_FILENO)
    close(ClientSocketError);

  disableStreamsBuffering();
  return true;
}

bool Server::disconnect() {
  if (ServerId == 0)
    return false;
  bool Success = true;
  if (close(STDIN_FILENO)) {
    Success = false;
    perror("disconnect: stdin");
  }
  if (close(STDOUT_FILENO)) {
    Success = false;
    perror("disconnect: stdout");
  }
  if (close(STDERR_FILENO)) {
    Success = false;
    perror("disconnect: stderr");
  }
  return Success;
}

void Server::done() {
  if (ServerSocketFdInOut != -1)
    close(ServerSocketFdInOut);
  ServerSocketFdInOut = -1;
  if (ServerSocketFdError != -1)
    close(ServerSocketFdError);
  ServerSocketFdError = -1;
  disconnect();
}

#endif // LLVM_ON_UNIX

#ifdef LLVM_ON_WIN32
static bool ConnectServer(HANDLE PipeHandle, LPOVERLAPPED OverLapped,
                          DWORD &ErrorCode) {
  if (!ConnectNamedPipe(PipeHandle, OverLapped)) {
    ErrorCode = GetLastError();
    if (ErrorCode != ERROR_PIPE_CONNECTED && ErrorCode != ERROR_IO_PENDING) {
      llvm::errs() << "ConnectServer: ConnectNamedPipe failed. last error "
                   << ErrorCode << "\n";
      return false;
    }
  }
  return true;
}

bool Server::connect() {
  assert(PipeInputHandle && PipeOutputHandle && PipeErrorHandle);
  // Connect using overlapped to prevent timing issues.
  OVERLAPPED PipeOLIn, PipeOLOut, PipeOLErr;
  ZeroMemory(&PipeOLIn, sizeof(OVERLAPPED));
  ZeroMemory(&PipeOLOut, sizeof(OVERLAPPED));
  ZeroMemory(&PipeOLErr, sizeof(OVERLAPPED));
  PipeOLIn.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  PipeOLOut.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  PipeOLErr.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (!PipeOLIn.hEvent || !PipeOLOut.hEvent || !PipeOLErr.hEvent)
    return false;

  DWORD ErrorCodeIn, ErrorCodeOut, ErrorCodeErr;
  // To avoid timing problems - output needs to be the last one connected in
  // server, as it is the first connected in client.
  if (!ConnectServer(PipeInputHandle, &PipeOLIn, ErrorCodeIn) ||
      !ConnectServer(PipeErrorHandle, &PipeOLErr, ErrorCodeErr) ||
      !ConnectServer(PipeOutputHandle, &PipeOLOut, ErrorCodeOut))
    return false;

  if (ErrorCodeIn == ERROR_IO_PENDING)
    WaitForSingleObject(PipeOLIn.hEvent, INFINITE);

  if (ErrorCodeErr == ERROR_IO_PENDING)
    WaitForSingleObject(PipeOLErr.hEvent, INFINITE);

  if (ErrorCodeOut == ERROR_IO_PENDING)
    WaitForSingleObject(PipeOLOut.hEvent, INFINITE);

  CloseHandle(PipeOLIn.hEvent);
  CloseHandle(PipeOLOut.hEvent);
  CloseHandle(PipeOLErr.hEvent);

  disableStreamsBuffering();

  return true;
}

// Return true on success
static bool DisconnectServer(HANDLE PipeHandle) {
  if (!DisconnectNamedPipe(PipeHandle)) {
    llvm::errs() << "DisconnectServer: DisconnectNamedPipe failed. last error "
                 << GetLastError() << "\n";
    return false;
  }
  return true;
}

bool Server::disconnect() {
  assert(PipeInputHandle && PipeOutputHandle && PipeErrorHandle);
  // Disconnect pipe to be ready for the next client or when done.
  // Output needs to be the last one closed in server, as it is the first
  // connected in client.
  bool Success = true;
  Success &= DisconnectServer(PipeInputHandle);
  Success &= DisconnectServer(PipeErrorHandle);
  Success &= DisconnectServer(PipeOutputHandle);
  return Success;
}

void Server::done() {
  if (!PipeInputHandle)
    return;
  disconnect();
  CloseHandle(PipeInputHandle);
  CloseHandle(PipeErrorHandle);
  CloseHandle(PipeOutputHandle);
}

#endif // LLVM_ON_WIN32
