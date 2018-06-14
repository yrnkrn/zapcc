// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "ErrorCouldNot.h"
#include "FindBaseName.h"

#ifdef _WIN32
#undef UNICODE
#include <windows.h>
#define PSAPI_VERSION 1
#include <psapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "psapi.lib")
#endif
#include <stdio.h>

// true on success.
static bool terminateProcess(HANDLE ProcessHandle) {
  if (!TerminateProcess(ProcessHandle, 0)) {
    fprintf(stderr, "terminateProcess: TerminateProcess error %lu\n",
            GetLastError());
    return false;
  }
  if (WaitForSingleObject(ProcessHandle, 10000)) {
    fprintf(stderr, "terminateProcess: WaitForSingleObject error %lu\n",
            GetLastError());
    return false;
  }
  return true;
} // terminateProcess

bool terminateAllCopiesOfHelper(const char *ExeName, bool JustFind) {
  const char *BaseExeName = findBaseName(ExeName);

  bool WasTerminated = false;
  DWORD OwnId = GetCurrentProcessId();

  DWORD ProcessIds[1024];
  DWORD BytesReturned;
  EnumProcesses(ProcessIds, sizeof(ProcessIds), &BytesReturned);
  DWORD NumProcesses = BytesReturned / sizeof(DWORD);

  for (unsigned i = 0; i < NumProcesses; i++) {
    // Don't suicide.
    if (ProcessIds[i] == OwnId)
      continue;
    HANDLE ProcessHandle =
        OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIds[i]);
    if (!ProcessHandle)
      continue;
    char FileName[1024];
    DWORD FileNameLen =
        GetModuleFileNameEx(ProcessHandle, NULL, FileName, sizeof(FileName));
    if (FileNameLen) {
      FileName[FileNameLen] = '\0';
      const char *BaseFileName = findBaseName(FileName);
      if (!_stricmp(BaseFileName, BaseExeName)) {
        if (JustFind) {
          CloseHandle(ProcessHandle);
          return true;
        }
        if (!terminateProcess(ProcessHandle)) {
          CloseHandle(ProcessHandle);
          ErrorCouldNot(FileName, "terminate");
          return false;
        }
        WasTerminated = true;
      }
    }
    CloseHandle(ProcessHandle);
  }
  return WasTerminated;
} // terminateAllCopiesOfHelper

#else
#include "StringConst.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

bool terminateAllCopiesOfHelper(const char *ExeName, bool JustFind) {
  // Scan /proc/####/status files and look for name of all running proccess
  // Terminate those matching given ExeName
  pid_t Pid;
  size_t i, j;
  char *Path = (char *)malloc(264);
  char Buf[128];
  FILE *StatusFile;
  bool KilledServer;
  do {
    KilledServer = false;
    DIR *d = opendir("/proc");
    if (d == NULL) {
      free(Path);
      return false;
    }
    struct dirent *Entry;
    while ((Entry = readdir(d)) != NULL) {
      if (Entry->d_name[0] == '.')
        continue;
      // check if name is digits only
      for (i = 0; isdigit(Entry->d_name[i]); i++)
        ;
      if (i < strlen(Entry->d_name))
        continue;
      strcpy(Path, "/proc/");
      strcat(Path, Entry->d_name);
      strcat(Path, "/status");
      StatusFile = fopen(Path, "r");
      if (StatusFile == NULL) {
        closedir(d);
        free(Path);
        return false;
      }
      do {
        if (fgets(Buf, 128, StatusFile) == NULL) {
          fclose(StatusFile);
          closedir(d);
          free(Path);
          return false;
        }
      } while (strncmp(Buf, "Name:", 5));
      fclose(StatusFile);
      for (j = 5; isspace(Buf[j]); j++)
        ; // skip spaces
      *strchr(Buf, '\n') = 0;
      if (!strcmp(&(Buf[j]), ExeName)) {
        sscanf(&(Path[6]), "%d", &Pid);
        if (JustFind) {
          closedir(d);
          free(Path);
          return true;
        }
        KilledServer = true;
        kill(Pid, SIGKILL);
      }
    }
    usleep(100000); // sleep 100 ms
    closedir(d);
  } while (KilledServer);
  free(Path);
  return true;
}
#endif // _WIN32

bool terminateAllCopiesOf(const char *ExeName) {
  return terminateAllCopiesOfHelper(ExeName, false);
} // terminateAllCopiesOf

bool isRunning(const char *ExeName) {
  return terminateAllCopiesOfHelper(ExeName, true);
} // isRunning

bool terminateAllServers() {
#ifdef _WIN32
  return terminateAllCopiesOf("zapccs.exe");
#else
  bool Status = terminateAllCopiesOf("zapccs");
  llvm::SmallString<1024> Name;
  for (int i = 0; i < 256; i++) {
    Name.clear();
    (SemaphorePrefix + llvm::Twine(i)).toVector(Name);
    if (sem_unlink(Name.c_str()) < 0 && errno != ENOENT)
      Status = false;
  }
  return Status;
#endif
}
