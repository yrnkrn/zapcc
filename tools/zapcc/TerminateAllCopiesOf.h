// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TERMINATEALLCOPIESOF_H
#define TERMINATEALLCOPIESOF_H

// true if a copy was terminated.
bool terminateAllCopiesOf(const char *ExeName);
bool isRunning(const char *ExeName);
bool terminateAllServers();
#endif // TERMINATEALLCOPIESOF_H
