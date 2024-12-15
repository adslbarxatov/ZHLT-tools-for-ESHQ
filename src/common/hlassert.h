#ifndef HLASSERT_H__
#define HLASSERT_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#ifdef SYSTEM_WIN32

#define assume(exp, message) {if (!(exp)) {Error("\nAssume '%s' failed\n at %s:%d\n %s\n\n", #exp, __FILE__, __LINE__, message);}}
#define hlassert(exp)

#endif

#ifdef SYSTEM_POSIX

#define assume(exp, message) {if (!(exp)) {Error("\nAssume '%s' failed\n\n", #exp, __FILE__, __LINE__, message);}}
#define hlassert(exp)

#endif

#endif
