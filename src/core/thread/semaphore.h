/*
 * Copyright (c) 2012-2017 Daniele Bartolini and individual contributors.
 * License: https://github.com/taylor001/crown/blob/master/LICENSE
 */

#pragma once

#include "error.h"
#include "mutex.h"
#include "platform.h"

#if CROWN_PLATFORM_POSIX
	#include <pthread.h>
#elif CROWN_PLATFORM_WINDOWS
	#include <windows.h>
	#include <limits.h>
#endif

namespace crown
{
/// Semaphore.
///
/// @ingroup Thread.
struct Semaphore
{
#if CROWN_PLATFORM_POSIX
	Mutex _mutex;
	pthread_cond_t _cond;
	s32 _count;
#elif CROWN_PLATFORM_WINDOWS
	HANDLE _handle;
#endif

	Semaphore()
#if CROWN_PLATFORM_POSIX
		: _count(0)
#elif CROWN_PLATFORM_WINDOWS
		: _handle(INVALID_HANDLE_VALUE)
#endif
	{
#if CROWN_PLATFORM_POSIX
		int err = pthread_cond_init(&_cond, NULL);
		CE_ASSERT(err == 0, "pthread_cond_init: errno = %d", err);
		CE_UNUSED(err);
#elif CROWN_PLATFORM_WINDOWS
		_handle = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
		CE_ASSERT(_handle != NULL, "CreateSemaphore: GetLastError = %d", GetLastError());
		CE_UNUSED(_handle);
#endif
	}

	~Semaphore()
	{
#if CROWN_PLATFORM_POSIX
		int err = pthread_cond_destroy(&_cond);
		CE_ASSERT(err == 0, "pthread_cond_destroy: errno = %d", err);
		CE_UNUSED(err);
#elif CROWN_PLATFORM_WINDOWS
		BOOL err = CloseHandle(_handle);
		CE_ASSERT(err != 0, "CloseHandle: GetLastError = %d", GetLastError());
		CE_UNUSED(err);
#endif
	}

	void post(u32 count = 1)
	{
#if CROWN_PLATFORM_POSIX
		ScopedMutex sm(_mutex);

		for (u32 i = 0; i < count; ++i)
		{
			int err = pthread_cond_signal(&_cond);
			CE_ASSERT(err == 0, "pthread_cond_signal: errno = %d", err);
			CE_UNUSED(err);
		}

		_count += count;
#elif CROWN_PLATFORM_WINDOWS
		BOOL err = ReleaseSemaphore(_handle, count, NULL);
		CE_ASSERT(err != 0, "ReleaseSemaphore: GetLastError = %d", GetLastError());
		CE_UNUSED(err);
#endif
	}

	void wait()
	{
#if CROWN_PLATFORM_POSIX
		ScopedMutex sm(_mutex);

		while (_count <= 0)
		{
			int err = pthread_cond_wait(&_cond, &(_mutex._mutex));
			CE_ASSERT(err == 0, "pthread_cond_wait: errno = %d", err);
			CE_UNUSED(err);
		}

		_count--;
#elif CROWN_PLATFORM_WINDOWS
		DWORD err = WaitForSingleObject(_handle, INFINITE);
		CE_ASSERT(err == WAIT_OBJECT_0, "WaitForSingleObject: GetLastError = %d", GetLastError());
		CE_UNUSED(err);
#endif
	}

private:

	// Disable copying
	Semaphore(const Semaphore& s);
	Semaphore& operator=(const Semaphore& s);
};

} // namespace crown
