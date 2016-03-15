//
//  VS_Mutex.h
//  VectorStorm
//
//  Created by Trevor Powell on 29/03/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef VS_MUTEX_H
#define VS_MUTEX_H

#ifdef __APPLE__
 #include <libkern/OSAtomic.h>
// use spinlocks on OSX, because mutexes are silly expensive here.
typedef OSSpinLock mutex_t;
#elif defined(UNIX)
#include <pthread.h>
typedef pthread_spinlock_t mutex_t;
#else
#include <windows.h>
typedef HANDLE mutex_t;
#undef PlaySound	// gah, Windows.

#endif

class vsMutex
{
    mutex_t   m_mutex;

public:
    vsMutex();
    ~vsMutex();

	bool TryLock();
    void Lock();
    void Unlock();
};

#endif // VS_MUTEX_H

