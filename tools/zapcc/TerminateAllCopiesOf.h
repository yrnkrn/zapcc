// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef TERMINATEALLCOPIESOF_H
#define TERMINATEALLCOPIESOF_H

// true if a copy was terminated.
bool terminateAllCopiesOf(const char *ExeName);
bool isRunning(const char *ExeName);
bool terminateAllServers();
#endif // TERMINATEALLCOPIESOF_H
