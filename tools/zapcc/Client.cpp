// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#undef UNICODE

#include "BasePort.h"
#include "Client.h"
#include "CommandLine.h"
#include "DisableStreamsBuffering.h"
#include "ExeSuffix.h"
#include "FindOption.h"
#include "GetExePath.h"
#include "StringConst.h"
#include "TerminateAllCopiesOf.h"
#include "TextFile.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
// Use tinycythread instead of C++11 threads to avoid enabling LLVM exceptions.
#include "tinycthread.h"

#ifdef LLVM_ON_UNIX
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#ifdef LLVM_ON_WIN32
#include <windows.h>
#endif

#define DEBUG_TYPE "zapcc-client"

static ScanCLIResult scanCommandLine(llvm::ArrayRef<const char *> Argv,
                                     std::string &CompileCommand,
                                     int &ServerId) {
  parseDebugFlags(Argv);
  ServerId = 0;
  CompileCommand.reserve(128 * Argv.size());
  llvm::SmallString<1024> CurrentDir;
  if (llvm::sys::fs::current_path(CurrentDir)) {
    llvm::errs() << "scanCommandLine error: no current dir?\n";
    return ScanCLIResult::Error;
  }
  CompileCommand = CurrentDir.c_str();
  CompileCommand += "\n";
  bool PrintCommands = false;
  bool ResetFlag = false;
  for (auto I = Argv.begin(), E = Argv.end(); I != E; ++I) {
    llvm::StringRef Arg(*I);
    assert(Arg.size());
    if (Arg == ServerIdSwitch) {
      ServerId = getIntOption(Argv, I - Argv.begin() + 1);
      if (ServerId < 0)
        return ScanCLIResult::Error;
      ++I;
      continue;
    } else if (Arg == PrintCommandsSwitch) {
      PrintCommands = true;
      continue;
    } else if (Arg == ResetSwitch) {
      ResetFlag = true;
    }
    CompileCommand += (*I);
    CompileCommand += "\n";
  }
  // Extra end of line signals termination of command.
  CompileCommand += "\n";
  if (PrintCommands) {
    writeRealTextFile("commands.txt", CompileCommand, true);
    return ScanCLIResult::CommandsRequest;
  }
  if (ResetFlag)
    return ScanCLIResult::ResetRequest;
  return ScanCLIResult::Success;
}

static bool startThread(thrd_t *Handle, thrd_start_t Func, void *Connections) {
  DEBUG(llvm::errs() << "startThread: calling thrd_create.\n");
  if (thrd_create(Handle, Func, Connections) != thrd_success) {
    llvm::errs() << "startThread error: thread create error\n";
    return false;
  }
  return true;
}

static int waitThread(thrd_t &Handle) {
  assert(Handle);
  int RetCode = 1;
  DEBUG(llvm::errs() << "waitThread: calling thrd_join.\n");
  thrd_join(Handle, &RetCode);
  DEBUG(llvm::errs() << "waitThread: thrd_join done, return code = " << RetCode
                     << "\n");
  return RetCode;
}

static void getServerName(const char *ExeName,
                          llvm::SmallString<1024> &ServerName) {
  const char *ClientName = GetExePath(ExeName);
  llvm::StringRef BaseClientName = llvm::sys::path::stem(ClientName);
  if (BaseClientName.endswith("++"))
    BaseClientName = BaseClientName.drop_back(2);
  ServerName = llvm::sys::path::parent_path(ClientName);
  llvm::sys::path::append(ServerName, BaseClientName + "s" EXESUFFIX);
}

static bool findExitCode(const char *Buffer, int NumBytesRead, int *ExitCode) {
  assert(NumBytesRead && "Called to find exit code for zero length buffer");
  char MaybeExitCode = *(Buffer + NumBytesRead - 1);
  if ((MaybeExitCode == 0) || (MaybeExitCode == 1) || (MaybeExitCode == 2)) {
    *ExitCode = MaybeExitCode;
    return true;
  }
  return false;
}

Client::Client(const char *ServerName, int ServerId)
    : ServerName(ServerName), ServerId(ServerId) {
  CName = nullptr;
#ifdef LLVM_ON_UNIX
  Connection.InputHandle = -1;
  Connection.OutputHandle = -1;
  Connection.ErrorHandle = -1;
  ServerSocketFdInOut = -1;
  ServerSocketFdError = -1;
  LockSocket = -1;
#endif
#ifdef LLVM_ON_WIN32
  Connection.InputHandle = nullptr;
  Connection.OutputHandle = nullptr;
  Connection.ErrorHandle = nullptr;

  CNameOutput = nullptr;
  CNameInput = nullptr;
  CNameError = nullptr;
#endif
}

bool Client::connect() {
  if (ServerId) {
    ConnectResult Result = connectToOne(true);
    if (Result == ConnectResult::Busy)
      llvm::errs() << "connectToOne error: Specific server " << ServerId
                   << " is busy.\n";
    return Result == ConnectResult::Success;
  } else
    return connectToAny();
}

Client::ConnectResult Client::connectToOne(bool UseSpecificServer) {
  if (UseSpecificServer)
    ServerId += MaxServers;
  setName();

  ConnectResult Result = obtainLock();
  if (Result != ConnectResult::Success)
    return Result;

  unsigned Wait = UseSpecificServer ? 800 : 1;
  Result = waitAndOpenConnection(Wait);
  if (Result == ConnectResult::NotFound) {
    Result = createPipeAndServer();
    if (Result == ConnectResult::Success) {
      // Server started, wait for it to be ready, second chance to connect.
      Wait = 4000;
      Result = waitAndOpenConnection(Wait);
    }
  }
  if (Result == ConnectResult::Unexpected)
    llvm::errs() << "connectToOne error: Unexpected error.\n";
  else if (Result == ConnectResult::NotFound)
    llvm::errs() << "connectToOne error: Server created, but not found.\n";
  return Result;
}

bool Client::connectToAny() {
  for (ServerId = 1; ServerId <= MaxServers; ++ServerId) {
    DEBUG(llvm::errs() << "connectToAny: trying to connect to server"
                       << ServerId << ".\n");
    ConnectResult Result = connectToOne(false);
    switch (Result) {
    case ConnectResult::Success: {
      DEBUG(llvm::errs() << "connectToAny: success.\n");
      return true;
    }
    case ConnectResult::NotFound:
    case ConnectResult::Unexpected:
      return false;
    case ConnectResult::Busy: {
      DEBUG(llvm::errs()
            << "connectToAny: Server busy, skipping to next server.\n");
      break;
    }
    }
  }
  llvm::errs() << "connectToAny error: scanned " << MaxServers
               << " servers but none worked!\n";
  return false;
}

// Platform-specific parts of Connection class.
#ifdef LLVM_ON_UNIX

static int readServerOutput(void *Connection) {
  int Fds[] = {((Client::ConnectionHandles *)Connection)->OutputHandle,
               ((Client::ConnectionHandles *)Connection)->ErrorHandle};
  FILE *Outs[] = {stdout, stderr};
  int ExitCode = Client::NotFoundExitCode;
  bool FdOpen[] = {true, true};
  while (FdOpen[0] || FdOpen[1]) {
    fd_set ReadSet;
    FD_ZERO(&ReadSet);
    int HighestFd = 0;
    for (unsigned Index : {0, 1}) {
      if (FdOpen[Index]) {
        FD_SET(Fds[Index], &ReadSet);
        HighestFd = HighestFd > Fds[Index] + 1 ? HighestFd : Fds[Index] + 1;
      }
    }
    if (select(HighestFd, &ReadSet, NULL, NULL, NULL) == -1) {
      perror("readServerOutput: select: ");
      break;
    }
    for (unsigned Index : {0, 1}) {
      if (FD_ISSET(Fds[Index], &ReadSet)) {
        ssize_t NumBytesRead;
        char Buffer[1024];
        if ((NumBytesRead = recv(Fds[Index], Buffer,
                                 sizeof(char) * sizeof(Buffer), 0)) <= 0) {
          if (close(Fds[Index])) {
            perror("readServerOutput: close output");
            ExitCode = 1;
          }
          FdOpen[Index] = false;
        } else {
          bool FoundExitCode = findExitCode(Buffer, NumBytesRead, &ExitCode);
          if (FoundExitCode)
            --NumBytesRead;
          fwrite(Buffer, sizeof(char), NumBytesRead, Outs[Index]);
          if (FoundExitCode)
            break;
        }
      }
    }
  }

  return ExitCode;
}

#if 0
static bool inputAvailable() {
  struct timeval tv = {};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET(0, &fds);
}
#endif

static int writeServerInput(void *Connection) {
  int Fd = ((Client::ConnectionHandles *)Connection)->InputHandle;
  if (isatty(fileno(stdin))) {
    DEBUG(llvm::errs() << "writeServerInput: stdin is tty\n");
  } else {
    char Buffer[1024];
    ssize_t NumBytesRead;
    while ((NumBytesRead = read(STDIN_FILENO, Buffer,
                                sizeof(char) * sizeof(Buffer))) > 0) {
      ssize_t NumBytesWritten = write(Fd, Buffer, sizeof(char) * NumBytesRead);
      // Bad file descriptor will happen if connection was already closed by now
      if (NumBytesWritten < 0 && errno != EBADF) {
        perror("writeServerInput: write failed");
        break;
      }
    }
    DEBUG(llvm::errs() << "writeServerInput: stdin closed\n");
  }
  // Input and output are on the same socket. close only the write half
  shutdown(Fd, SHUT_WR);
  return 0;
}

// Return true on success.
static bool daemonize(int FdOut, int FdErr) {
  // Steps below are to daemonize the process, take from ubuntu tutorial.
  // Change the file mode mask.
  umask(0);
  // Create a new SID for the child process.
  pid_t sid = setsid();
  if (sid < 0) {
    perror("daemonize: setsid");
    return false;
  }
  // Change the current working directory.
  if ((chdir("/")) < 0) {
    perror("daemonize: chdir");
    return false;
  }

  // Close all inherited file descriptors
  // If we inherit a pipe and keep it open, the calling process hangs
  // FIXME: We can use /proc/self to close all open files?
  for (int Fd = STDERR_FILENO + 1; Fd <= 60; ++Fd)
    if (Fd != FdOut && Fd != FdErr)
      close(Fd);

  return true;
}

Client::~Client() {
  Connection.InputHandle = -1;
  Connection.OutputHandle = -1;
  Connection.ErrorHandle = -1;
  releaseLock();
}

void Client::setName() {
  Name.clear();
  (SemaphorePrefix + llvm::Twine(ServerId)).toVector(Name);
  CName = Name.c_str();
}

Client::ConnectResult Client::obtainLock() {
  assert(LockSocket < 0 && "lock obtained twice");

  DEBUG(llvm::errs() << "obtainLock: trying to obtain lock for ServerId = "
                     << ServerId << " Name = " << CName << "\n");
  LockSocket = ::open(CName, O_CREAT | O_WRONLY, S_IRWXU);
  if (LockSocket < 0) {
    llvm::errs() << "obtainLock: ::open for ServerId = " << ServerId
                 << " failed " << strerror(errno) << "\n";
    return ConnectResult::Unexpected;
  }

  // FIXME: are locks inheritied to the server we create?
  // http://man7.org/linux/man-pages/man2/fcntl.2.html
  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid(); // Our PID.
  if (fcntl(LockSocket, F_SETLK, &fl) < 0) {
    int Err = errno;
    releaseLock();
    if (Err == EAGAIN || Err == EACCES) {
      DEBUG(llvm::errs() << "obtainLock: fcntl EAGAIN EACCES for ServerId = "
                         << ServerId << " failed " << strerror(Err) << "\n");
      return ConnectResult::Busy;
    }
    llvm::errs() << "obtainLock: fcntl failed for ServerId = " << ServerId
                 << strerror(Err) << "\n";
    return ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "obtainLock: lock success for ServerId = " << ServerId
                     << ".\n");
  return ConnectResult::Success;
}

Client::ConnectResult Client::waitAndOpenConnection(unsigned ms) {
  return open();
}

void Client::releaseLock() {
  if (LockSocket > 0)
    close(LockSocket);
  LockSocket = -1;
}

Client::ConnectResult Client::createPipeAndServer() {
  // Create socket.
  ServerSocketFdInOut = socket(AF_INET, SOCK_STREAM, 0);
  if (ServerSocketFdInOut == -1) {
    perror("create: ServerSocketFdOut");
    return ConnectResult::Unexpected;
  }
  ServerSocketFdError = socket(AF_INET, SOCK_STREAM, 0);
  if (ServerSocketFdError == -1) {
    perror("create: ServerSocketFdError");
    return ConnectResult::Unexpected;
  }

  // Reuse already bound address/port (if possible).
  int One = 1;
  if (setsockopt(ServerSocketFdInOut, SOL_SOCKET, SO_REUSEADDR, &One,
                 sizeof(int)) < 0 ||
      setsockopt(ServerSocketFdError, SOL_SOCKET, SO_REUSEADDR, &One,
                 sizeof(int)) < 0) {
    perror("create: setsockopt");
    return ConnectResult::Unexpected;
  }

  // Prepare the sockaddr_in structure.
  struct sockaddr_in AddrOut;
  AddrOut.sin_family = AF_INET;
  AddrOut.sin_addr.s_addr = INADDR_ANY;
  AddrOut.sin_port = htons(ZapccBasePort + ServerId * 2);

  struct sockaddr_in AddrError;
  AddrError.sin_family = AF_INET;
  AddrError.sin_addr.s_addr = INADDR_ANY;
  AddrError.sin_port = htons(ZapccBasePort + ServerId * 2 + 1);

  if (::bind(ServerSocketFdInOut, (struct sockaddr *)&AddrOut,
             sizeof(AddrOut)) < 0 ||
      ::bind(ServerSocketFdError, (struct sockaddr *)&AddrError,
             sizeof(AddrError)) < 0) {
    perror("create: bind");
    return ConnectResult::Unexpected;
  }

  if (listen(ServerSocketFdInOut, 1) < 0 ||
      listen(ServerSocketFdError, 1) < 0) {
    perror("create: listen");
    return ConnectResult::Unexpected;
  }

  if (!startServer())
    return ConnectResult::Unexpected;
  close(ServerSocketFdInOut);
  close(ServerSocketFdError);
  return ConnectResult::Success;
}

bool Client::startServer() {
  DEBUG(llvm::errs() << "startServer: ServerName = " << ServerName
                     << ", ServerId = " << ServerId
                     << " FdInOut = " << ServerSocketFdInOut
                     << " FdError = " << ServerSocketFdError << ".\n");
  pid_t Pid = fork();
  if (Pid < 0) {
    perror("startServer: fork");
    return false;
  }
  if (Pid == 0) {
    DEBUG(llvm::errs() << "startServer: fork success (child).\n");
    // Child process - run zapcc.
    if (!daemonize(ServerSocketFdInOut, ServerSocketFdError)) {
      llvm::errs() << "startServer error: daemonize failed.\n";
      return false;
    }
    std::string ServerIdString = std::to_string(ServerId);
    std::string ServerSocketFdInOutString = std::to_string(ServerSocketFdInOut);
    std::string ServerSocketFdErrorString = std::to_string(ServerSocketFdError);
    int Ret = execl(ServerName, ServerName, ServerIdString.c_str(),
                    ServerSocketFdInOutString.c_str(),
                    ServerSocketFdErrorString.c_str(), nullptr);
    // execl doesn't return unless there is an error.
    llvm::errs() << "startServer error: execl failed return code " << Ret
                 << ".\n";
    // exit instead of return since this is the child process.
    exit(1);
  }
  DEBUG(llvm::errs() << "startServer: fork success (father), ServerId = "
                     << ServerId << ".\n");
  return true;
}

Client::ConnectResult Client::open() {
  // Close previous sockets if created.
  if (Connection.InputHandle > 0)
    close(Connection.InputHandle);
  if (Connection.OutputHandle > 0)
    close(Connection.OutputHandle);
  if (Connection.ErrorHandle > 0)
    close(Connection.ErrorHandle);

  if ((Connection.InputHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("open: InputHandle");
    return ConnectResult::Unexpected;
  }

  Connection.OutputHandle = Connection.InputHandle;

  if ((Connection.ErrorHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("open: ErrorHandle");
    return ConnectResult::Unexpected;
  }

  struct sockaddr_in AddrOut;
  memset(&AddrOut, 0, sizeof(AddrOut));
  AddrOut.sin_family = AF_INET;
  AddrOut.sin_addr.s_addr = inet_addr("127.0.0.1");
  AddrOut.sin_port = htons(ZapccBasePort + ServerId * 2);

  struct sockaddr_in AddrErr;
  memset(&AddrErr, 0, sizeof(AddrErr));
  AddrErr.sin_family = AF_INET;
  AddrErr.sin_addr.s_addr = inet_addr("127.0.0.1");
  AddrErr.sin_port = htons(ZapccBasePort + ServerId * 2 + 1);

  if (::connect(Connection.InputHandle, (struct sockaddr *)&AddrOut,
                sizeof(AddrOut))) {
    DEBUG(llvm::errs() << "open: connect input ServerId = " << ServerId
                       << " failed, " << strerror(errno) << ".\n");
    if (errno == ECONNREFUSED)
      return ConnectResult::NotFound;
    llvm::errs() << "open: connect input ServerId = " << ServerId << " failed, "
                 << strerror(errno) << ".\n";
    return ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "open: connect input ServerId = " << ServerId
                     << " success.\n");

  if (::connect(Connection.ErrorHandle, (struct sockaddr *)&AddrErr,
                sizeof(AddrErr))) {
    llvm::errs() << "open error: connect error ServerId = " << ServerId
                 << " failed, " << strerror(errno) << ".\n";
    return ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "open: connect error ServerId = " << ServerId
                     << " success.\n");
  return ConnectResult::Success;
}

bool Client::send(const std::string &Command) {
  assert(Connection.InputHandle >= 0 && "Can not write to invalid FD");
  // Note: Command is terminated by an extra \n, do not add another one.
  if (::send(Connection.InputHandle, Command.c_str(), Command.size(), 0) < 0) {
    llvm::errs() << "send error: FD=" << Connection.InputHandle
                 << " errocode=" << errno << ".\n";
    return false;
  }
  return true;
}

#endif // LLVM_ON_UNIX

#ifdef LLVM_ON_WIN32

static void closeHandle(HANDLE &h) {
  if (h && !CloseHandle(h))
    llvm::errs() << "closeHandle error: CloseHandle handle = " << h
                 << " GetLastError = " << GetLastError() << ".\n";
  h = nullptr;
}

static int readServerOutput(void *Connection) {
  HANDLE ReadHandles[] = {
      ((Client::ConnectionHandles *)Connection)->OutputHandle,
      ((Client::ConnectionHandles *)Connection)->ErrorHandle};
  HANDLE StdHandles[] = {GetStdHandle(STD_OUTPUT_HANDLE),
                         GetStdHandle(STD_ERROR_HANDLE)};
  OVERLAPPED PipeOL[2] = {};
  for (unsigned BufferIndex : {0, 1}) {
    ZeroMemory(&PipeOL[BufferIndex], sizeof(OVERLAPPED));
    PipeOL[BufferIndex].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!PipeOL[BufferIndex].hEvent) {
      llvm::errs() << "readServerOutput error: CreateEvent GetLastError = "
                   << GetLastError();
      return 3;
    }
  }
  HANDLE PipeOLHandles[] = {PipeOL[0].hEvent, PipeOL[1].hEvent};
  bool ShouldRead[] = {true, true};
  char Buffers[2][1024];

  int ExitCode = Client::NotFoundExitCode;
  while (true) {
    for (unsigned BufferIndex : {0, 1}) {
      if (!ShouldRead[BufferIndex])
        continue;
      if (!ReadFile(ReadHandles[BufferIndex], Buffers[BufferIndex],
                    sizeof(Buffers[BufferIndex]), NULL, &PipeOL[BufferIndex])) {
        DWORD Err = GetLastError();
        if (Err == ERROR_PIPE_NOT_CONNECTED) {
          // Normal server disconnect.
          DEBUG(llvm::errs() << "readServerOutput: ReadFile GetLastError = "
                                "ERROR_PIPE_NOT_CONNECTED\n");
          break;
        }
        if (Err != ERROR_IO_PENDING) {
          llvm::errs() << "readServerOutput error: ReadFile GetLastError = "
                       << Err << "\n";
          break;
        }
      }
      ShouldRead[BufferIndex] = false;
    }

    DWORD ConnectResult =
        WaitForMultipleObjects(sizeof(PipeOLHandles) / sizeof(PipeOLHandles[0]),
                               PipeOLHandles, FALSE, INFINITE);
    if (ConnectResult == WAIT_FAILED) {
      DEBUG(llvm::errs() << "readServerOutput: WaitForMultipleObjects "
                            "WAIT_FAILED GetLastError = "
                         << GetLastError() << "\n");
      break;
    } else if (ConnectResult == WAIT_OBJECT_0 ||
               ConnectResult == WAIT_OBJECT_0 + 1) {
      unsigned BufferIndex = ConnectResult - WAIT_OBJECT_0;
      ShouldRead[BufferIndex] = true;
      DWORD NumBytesRead = 0;
      GetOverlappedResult(ReadHandles[BufferIndex], &PipeOL[BufferIndex],
                          &NumBytesRead, TRUE);
      if (!NumBytesRead)
        break;
      bool FoundExitCode =
          findExitCode(Buffers[BufferIndex], NumBytesRead, &ExitCode);
      if (FoundExitCode)
        --NumBytesRead;
      DWORD NumBytesWritten;
      if (!WriteFile(StdHandles[BufferIndex], &Buffers[BufferIndex],
                     NumBytesRead, &NumBytesWritten, NULL)) {
        DEBUG(llvm::errs() << "readServerOutput: WriteFile GetLastError = "
                           << GetLastError() << "\n");
        break;
      }
      if (FoundExitCode)
        break;
    } else {
      llvm::errs()
          << "readServerOutput error: WaitForMultipleObjects GetLastError = "
          << ConnectResult << ".\n";
    }
  } // while (true)
  for (unsigned BufferIndex : {0, 1})
    closeHandle(PipeOL[BufferIndex].hEvent);
  return ExitCode;
}

static int writeServerInput(void *Connection) {
  HANDLE StdinHandle = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE &ServerInput = ((Client::ConnectionHandles *)Connection)->InputHandle;

  if (GetFileType(StdinHandle) == FILE_TYPE_CHAR) {
    DEBUG(llvm::errs() << "writeServerInput: stdin is tty\n");
  } else {
    while (true) {
      char Buffer[1024];
      DWORD NumBytesRead = 0;
      if (!ReadFile(StdinHandle, Buffer, sizeof(Buffer), &NumBytesRead, NULL))
        break;
      if (!NumBytesRead)
        break;
      DWORD NumBytesWritten;
      if (!WriteFile(ServerInput, Buffer, NumBytesRead, &NumBytesWritten,
                     NULL)) {
        DEBUG(llvm::errs() << "writeServerInput: WriteFile GetLastError = "
                           << GetLastError() << "\n");
        break;
      }
    }
  }
  closeHandle(ServerInput);
  return 0;
}

Client::~Client() {
  closeHandle(Connection.InputHandle);
  closeHandle(Connection.OutputHandle);
  closeHandle(Connection.ErrorHandle);
}

void Client::setName() {
  NameOutput.clear();
  NameInput.clear();
  NameError.clear();
  (NamedPipeOutPrefix + llvm::Twine(ServerId)).toVector(NameOutput);
  (NamedPipeInPrefix + llvm::Twine(ServerId)).toVector(NameInput);
  (NamedPipeErrPrefix + llvm::Twine(ServerId)).toVector(NameError);
  CNameInput = NameInput.c_str();
  CNameOutput = NameOutput.c_str();
  CNameError = NameError.c_str();
  CName = NameOutput.c_str();
}

Client::ConnectResult Client::obtainLock() { return ConnectResult::Success; }

void Client::releaseLock() {}

Client::ConnectResult Client::waitAndOpenConnection(unsigned ms) {
  if (!WaitNamedPipe(CName, ms)) {
    DWORD LastError = GetLastError();
    DEBUG(llvm::errs() << "waitAndOpenConnection: WaitNamedPipe on '" << CName
                       << "' failed. GetLastError "
                          "(ERROR_FILE_NOT_FOUND=2,ERROR_SEM_"
                          "TIMEOUT=121) = "
                       << LastError << "\n");
    if (LastError == ERROR_FILE_NOT_FOUND)
      return ConnectResult::NotFound;
    if (LastError == ERROR_SEM_TIMEOUT)
      return ConnectResult::Busy;
    DEBUG(llvm::errs()
          << "waitAndOpenConnection:  WaitNamedPipe unexpected GetLastError = "
          << LastError << ".\n");
    return ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "waitAndOpenConnection: WaitNamedPipe on '" << CName
                     << "' success.\n");
  return open();
}

static Client::ConnectResult createFile(const char *FileName, bool Read,
                                        HANDLE &h) {
  DWORD DesiredAccess = Read ? GENERIC_READ : GENERIC_WRITE;
  DWORD ShareMode = Read ? FILE_SHARE_READ : FILE_SHARE_WRITE;
  h = CreateFile(FileName, DesiredAccess, ShareMode, NULL, OPEN_EXISTING,
                 FILE_FLAG_OVERLAPPED, NULL);
  if (h == INVALID_HANDLE_VALUE) {
    DWORD LastError = GetLastError();
    if (LastError == ERROR_PIPE_BUSY) {
      DEBUG(llvm::errs() << "open: CreateFile '" << FileName
                         << "' fail, GetLastError = ERROR_PIPE_BUSY.\n");
      return Client::ConnectResult::Busy;
    }
    llvm::errs() << "open error: CreateFile '" << FileName
                 << "' fail, GetLastError = " << LastError << ".\n";
    return Client::ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "open error: CreateFile '" << FileName
                     << "' success.\n");
  return Client::ConnectResult::Success;
}

Client::ConnectResult Client::open() {
  // If we got to here, wait was successful so we can open the pipe
  // unless another client grabbed it between WaitNamedPipe and CreateFile.
  ConnectResult Result = createFile(CNameOutput, true, Connection.OutputHandle);
  if (Result != ConnectResult::Success)
    return Result;
  Result = createFile(CNameError, true, Connection.ErrorHandle);
  if (Result != ConnectResult::Success)
    return Result;
  return createFile(CNameInput, false, Connection.InputHandle);
}

static Client::ConnectResult createNamedPipe(const char *PipeName,
                                             bool Outbound, HANDLE &h) {
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  DWORD PipeMode = Outbound ? PIPE_ACCESS_OUTBOUND : PIPE_ACCESS_INBOUND;
  h = CreateNamedPipe(
      PipeName, PipeMode | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
      PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 0, &sa);
  if (h == INVALID_HANDLE_VALUE) {
    DWORD LastError = GetLastError();
    if (LastError == ERROR_PIPE_BUSY) {
      DEBUG(llvm::errs() << "create: CreateNamedPipe '" << PipeName
                         << "' fail, GetLastError = ERROR_PIPE_BUSY.\n");
      return Client::ConnectResult::Busy;
    }
    llvm::errs() << "create error: CreateNamedPipe '" << PipeName
                 << "' fail, GetLastError = " << LastError << ".\n";
    return Client::ConnectResult::Unexpected;
  }
  DEBUG(llvm::errs() << "create error: CreateNamedPipe '" << PipeName
                     << "' success.\n");
  return Client::ConnectResult::Success;
}

Client::ConnectResult Client::createPipeAndServer() {
  ConnectResult Result;
  Result = createNamedPipe(CNameOutput, true, Connection.OutputHandle);
  if (Result != ConnectResult::Success)
    return Result;
  Result = createNamedPipe(CNameInput, false, Connection.InputHandle);
  if (Result != ConnectResult::Success)
    return Result;
  Result = createNamedPipe(CNameError, true, Connection.ErrorHandle);
  if (Result != ConnectResult::Success)
    return Result;
  if (!startServer())
    return ConnectResult::Unexpected;
  return ConnectResult::Success;
}

// https://blogs.msdn.microsoft.com/oldnewthing/20111216-00/?p=8873
static BOOL CreateProcessWithExplicitHandles(
    LPCTSTR lpApplicationName, LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
    // here is the new stuff
    DWORD cHandlesToInherit, HANDLE *rgHandlesToInherit) {
  BOOL fSuccess;
  BOOL fInitialized = FALSE;
  SIZE_T size = 0;
  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList = NULL;

  fSuccess = cHandlesToInherit < 0xFFFFFFFF / sizeof(HANDLE) &&
             lpStartupInfo->cb == sizeof(*lpStartupInfo);
  if (!fSuccess) {
    SetLastError(ERROR_INVALID_PARAMETER);
  }
  if (fSuccess) {
    fSuccess = InitializeProcThreadAttributeList(NULL, 1, 0, &size) ||
               GetLastError() == ERROR_INSUFFICIENT_BUFFER;
  }
  if (fSuccess) {
    lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(
        HeapAlloc(GetProcessHeap(), 0, size));
    fSuccess = lpAttributeList != NULL;
  }
  if (fSuccess) {
    fSuccess = InitializeProcThreadAttributeList(lpAttributeList, 1, 0, &size);
  }
  if (fSuccess) {
    fInitialized = TRUE;
    fSuccess = UpdateProcThreadAttribute(
        lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        rgHandlesToInherit, cHandlesToInherit * sizeof(HANDLE), NULL, NULL);
  }
  if (fSuccess) {
    STARTUPINFOEX info;
    ZeroMemory(&info, sizeof(info));
    info.StartupInfo = *lpStartupInfo;
    info.StartupInfo.cb = sizeof(info);
    info.lpAttributeList = lpAttributeList;
    fSuccess = CreateProcess(
        lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles,
        dwCreationFlags | EXTENDED_STARTUPINFO_PRESENT, lpEnvironment,
        lpCurrentDirectory, &info.StartupInfo, lpProcessInformation);
  }

  if (fInitialized)
    DeleteProcThreadAttributeList(lpAttributeList);
  if (lpAttributeList)
    HeapFree(GetProcessHeap(), 0, lpAttributeList);
  return fSuccess;
}

bool Client::startServer() {
  STARTUPINFO si;
  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.hStdInput = Connection.InputHandle;
  si.hStdOutput = Connection.OutputHandle;
  si.hStdError = Connection.ErrorHandle;
  si.dwFlags |= STARTF_USESTDHANDLES;
  // Eclipse understand a process is terminated when its std* handles
  // are closed. If we inherit std* to server they will not be closed
  // and eclipse will think we're still running.
  // The server don't need these anyhow.
  HANDLE StdinHandle = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE StdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE StderrHandle = GetStdHandle(STD_ERROR_HANDLE);
  SetHandleInformation(StdinHandle, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(StdoutHandle, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(StderrHandle, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
  llvm::SmallString<1024> CmdLine;
  (llvm::Twine(ServerName) + " " + llvm::Twine(ServerId) + " 0 0")
      .toVector(CmdLine);
  BOOL Success = CreateProcessWithExplicitHandles(
      NULL, (LPSTR) const_cast<char *>(CmdLine.c_str()), NULL, NULL, TRUE,
      CREATE_NO_WINDOW, NULL, NULL, &si, &pi, 3, &si.hStdInput);
  if (Success) {
    // We don't need these handles.
    closeHandle(pi.hProcess);
    closeHandle(pi.hThread);
    DEBUG(llvm::errs() << "startServer: CreateProcess success.\n");
  } else {
    llvm::errs() << "startServer: CreateProcess '" << CmdLine
                 << "' GetLastError = " << GetLastError() << ".\n";
  }
  closeHandle(Connection.InputHandle);
  closeHandle(Connection.OutputHandle);
  closeHandle(Connection.ErrorHandle);
  return Success;
}

bool Client::send(const std::string &Command) {
  bool Success = true;
  DWORD BytesWrriten;
  if (WriteFile(Connection.InputHandle, Command.data(), Command.size(),
                &BytesWrriten, NULL)) {
    DEBUG(llvm::errs() << "send: WriteFile success.\n");
  } else {
    DWORD LastError = GetLastError();
    DEBUG(
        llvm::errs() << "send: WriteFile GetLastError (ERROR_IO_PENDING=997) = "
                     << LastError << ".\n");
    if (LastError != ERROR_IO_PENDING) {
      Success = false;
    }
  }
  return Success;
} // send

#endif // LLVM_ON_WIN32

// Return 1 for error.
int startServerAndSend(llvm::ArrayRef<const char *> Argv, const char *Argv0) {
  disableStreamsBuffering();
  llvm::StringRef ExePath = GetExePath(Argv0);
  std::string CompileCommand;
  int ServerId;
  ScanCLIResult Result = scanCommandLine(Argv, CompileCommand, ServerId);
  if (Result == ScanCLIResult::Error)
    return 1;
  if (Result == ScanCLIResult::CommandsRequest)
    return 0;
  if (Result == ScanCLIResult::ResetRequest && !ServerId) {
    // If there is no server id, just terminate all servers
    terminateAllServers();
    return 0;
  }
  llvm::SmallString<1024> ServerName;
  getServerName(Argv0, ServerName);
  Client client(ServerName.c_str(), ServerId);
  if (!client.connect())
    return 1;
  thrd_t StdoutThread = 0;
  if (!startThread(&StdoutThread, readServerOutput, &client.Connection))
    return 1;
  if (!client.send(CompileCommand))
    return 1;
  thrd_t StdinThread = 0;
  if (!strcmp(Argv.back(), "-")) {
    if (!startThread(&StdinThread, writeServerInput, &client.Connection))
      return 1;
  }
  int ExitCode = waitThread(StdoutThread);
  // If no server exit code, assume it crashed and error.
  if (ExitCode == Client::NotFoundExitCode)
    return 1;
  DEBUG(llvm::errs() << "readServerOutput: found exitcode = " << ExitCode
                     << "\n");
  if (ExitCode)
    return ExitCode;
  if (StdinThread) {
    if (int RetCode = waitThread(StdinThread))
      return RetCode;
  }
  DEBUG(llvm::errs() << "startServerAndSend: success.\n");
  return 0;
}
