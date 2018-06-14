#pragma once
extern "C" __inline void pthread_mutex_lock() {}
static __typeof(pthread_mutex_lock) __gthrw_pthread_equal
    __attribute__((__weakref__("pthread_mutex_lock")));
