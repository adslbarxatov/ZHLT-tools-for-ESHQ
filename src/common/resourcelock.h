#ifndef RESOURCELOCK_H__
#define RESOURCELOCK_H__

#if _MSC_VER >= 1000
#pragma once
#endif

extern void		*CreateResourceLock (int LockNumber);
extern void		ReleaseResourceLock (void **lock);

#endif
