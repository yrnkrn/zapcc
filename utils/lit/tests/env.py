#!/usr/bin/env python
# RUN: env KEY=VAL %s | FileCheck %s --check-prefix=CHECK-SINGLE
# CHECK-SINGLE: VAL
# CHECK-SINGLE: None
# RUN: env KEY=VAL TZ=GMT %s | FileCheck %s --check-prefix CHECK-MULTI
# CHECK-MULTI: VAL
# CHECK-MULTI: GMT
# RUN: env %s

import os
print os.environ.get('KEY')
print os.environ.get('TZ')
