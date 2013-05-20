/*
 * libev event processing core, watcher management
 *
 * Copyright (c) 2007,2008,2009,2010,2011,2012 Marc Alexander Lehmann <libev@schmorp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 *
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License ("GPL") version 2 or any later version,
 * in which case the provisions of the GPL are applicable instead of
 * the above. If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the BSD license, indicate your decision
 * by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete the
 * provisions above, a recipient may use your version of this file under
 * either the BSD or the GPL.
 */

/* this big block deduces configuration from config.h */
#ifndef EV_STANDALONE
# ifdef EV_CONFIG_H
#  include EV_CONFIG_H
# else
#  include "config.h"
# endif

#if HAVE_FLOOR
# ifndef EV_USE_FLOOR
#  define EV_USE_FLOOR 1
# endif
#endif

# if HAVE_CLOCK_SYSCALL
#  ifndef EV_USE_CLOCK_SYSCALL
#   define EV_USE_CLOCK_SYSCALL 1
#   ifndef EV_USE_REALTIME
#    define EV_USE_REALTIME  0
#   endif
#   ifndef EV_USE_MONOTONIC
#    define EV_USE_MONOTONIC 1
#   endif
#  endif
# elif !defined EV_USE_CLOCK_SYSCALL
#  define EV_USE_CLOCK_SYSCALL 0
# endif

# if HAVE_CLOCK_GETTIME
#  ifndef EV_USE_MONOTONIC
#   define EV_USE_MONOTONIC 1
#  endif
#  ifndef EV_USE_REALTIME
#   define EV_USE_REALTIME  0
#  endif
# else
#  ifndef EV_USE_MONOTONIC
#   define EV_USE_MONOTONIC 0
#  endif
#  ifndef EV_USE_REALTIME
#   define EV_USE_REALTIME  0
#  endif
# endif

# if HAVE_NANOSLEEP
#  ifndef EV_USE_NANOSLEEP
#    define EV_USE_NANOSLEEP EV_FEATURE_OS
#  endif
# else
#   undef EV_USE_NANOSLEEP
#   define EV_USE_NANOSLEEP 0
# endif

# if HAVE_SELECT && HAVE_SYS_SELECT_H
#  ifndef EV_USE_SELECT
#   define EV_USE_SELECT EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_SELECT
#  define EV_USE_SELECT 0
# endif

# if HAVE_POLL && HAVE_POLL_H
#  ifndef EV_USE_POLL
#   define EV_USE_POLL EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_POLL
#  define EV_USE_POLL 0
# endif

# if HAVE_EPOLL_CTL && HAVE_SYS_EPOLL_H
#  ifndef EV_USE_EPOLL
#   define EV_USE_EPOLL EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_EPOLL
#  define EV_USE_EPOLL 0
# endif

# if HAVE_KQUEUE && HAVE_SYS_EVENT_H
#  ifndef EV_USE_KQUEUE
#   define EV_USE_KQUEUE EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_KQUEUE
#  define EV_USE_KQUEUE 0
# endif

# if HAVE_PORT_H && HAVE_PORT_CREATE
#  ifndef EV_USE_PORT
#   define EV_USE_PORT EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_PORT
#  define EV_USE_PORT 0
# endif

# if HAVE_INOTIFY_INIT && HAVE_SYS_INOTIFY_H
#  ifndef EV_USE_INOTIFY
#   define EV_USE_INOTIFY EV_FEATURE_OS
#  endif
# else
#  undef EV_USE_INOTIFY
#  define EV_USE_INOTIFY 0
# endif

# if HAVE_SIGNALFD && HAVE_SYS_SIGNALFD_H
#  ifndef EV_USE_SIGNALFD
#   define EV_USE_SIGNALFD EV_FEATURE_OS
#  endif
# else
#  undef EV_USE_SIGNALFD
#  define EV_USE_SIGNALFD 0
# endif

# if HAVE_EVENTFD
#  ifndef EV_USE_EVENTFD
#   define EV_USE_EVENTFD EV_FEATURE_OS
#  endif
# else
#  undef EV_USE_EVENTFD
#  define EV_USE_EVENTFD 0
# endif

#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>

#include <stdio.h>

#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>

#include <signal.h>

#ifdef EV_H
# include EV_H
#else
# include "ev.h"
#endif

#if EV_NO_THREADS
# undef EV_NO_SMP
# define EV_NO_SMP 1
# undef ECB_NO_THREADS
# define ECB_NO_THREADS 1
#endif
#if EV_NO_SMP
# undef EV_NO_SMP
# define ECB_NO_SMP 1
#endif

#ifndef _WIN32
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>
#else
# include <io.h>
# define WIN32_LEAN_AND_MEAN
# include <winsock2.h>
# include <windows.h>
# ifndef EV_SELECT_IS_WINSOCKET
#  define EV_SELECT_IS_WINSOCKET 1
# endif
# undef EV_AVOID_STDIO
#endif

/* OS X, in its infinite idiocy, actually HARDCODES
 * a limit of 1024 into their select. Where people have brains,
 * OS X engineers apparently have a vacuum. Or maybe they were
 * ordered to have a vacuum, or they do anything for money.
 * This might help. Or not.
 */
#define _DARWIN_UNLIMITED_SELECT 1

/* this block tries to deduce configuration from header-defined symbols and defaults */

/* try to deduce the maximum number of signals on this platform */
#if defined EV_NSIG
/* use what's provided */
#elif defined NSIG
# define EV_NSIG (NSIG)
#elif defined _NSIG
# define EV_NSIG (_NSIG)
#elif defined SIGMAX
# define EV_NSIG (SIGMAX+1)
#elif defined SIG_MAX
# define EV_NSIG (SIG_MAX+1)
#elif defined _SIG_MAX
# define EV_NSIG (_SIG_MAX+1)
#elif defined MAXSIG
# define EV_NSIG (MAXSIG+1)
#elif defined MAX_SIG
# define EV_NSIG (MAX_SIG+1)
#elif defined SIGARRAYSIZE
# define EV_NSIG (SIGARRAYSIZE) /* Assume ary[SIGARRAYSIZE] */
#elif defined _sys_nsig
# define EV_NSIG (_sys_nsig) /* Solaris 2.5 */
#else
# error "unable to find value for NSIG, please report"
/* to make it compile regardless, just remove the above line, */
/* but consider reporting it, too! :) */
# define EV_NSIG 65
#endif

#ifndef EV_USE_FLOOR
# define EV_USE_FLOOR 0
#endif

#ifndef EV_USE_CLOCK_SYSCALL
# if __linux && __GLIBC__ >= 2
#  define EV_USE_CLOCK_SYSCALL EV_FEATURE_OS
# else
#  define EV_USE_CLOCK_SYSCALL 0
# endif
#endif

#ifndef EV_USE_MONOTONIC
# if defined _POSIX_MONOTONIC_CLOCK && _POSIX_MONOTONIC_CLOCK >= 0
#  define EV_USE_MONOTONIC EV_FEATURE_OS
# else
#  define EV_USE_MONOTONIC 0
# endif
#endif

#ifndef EV_USE_REALTIME
# define EV_USE_REALTIME !EV_USE_CLOCK_SYSCALL
#endif

#ifndef EV_USE_NANOSLEEP
# if _POSIX_C_SOURCE >= 199309L
#  define EV_USE_NANOSLEEP EV_FEATURE_OS
# else
#  define EV_USE_NANOSLEEP 0
# endif
#endif

#ifndef EV_USE_SELECT
# define EV_USE_SELECT EV_FEATURE_BACKENDS
#endif

#ifndef EV_USE_POLL
# ifdef _WIN32
#  define EV_USE_POLL 0
# else
#  define EV_USE_POLL EV_FEATURE_BACKENDS
# endif
#endif

#ifndef EV_USE_EPOLL
# if __linux && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 4))
#  define EV_USE_EPOLL EV_FEATURE_BACKENDS
# else
#  define EV_USE_EPOLL 0
# endif
#endif

#ifndef EV_USE_KQUEUE
# define EV_USE_KQUEUE 0
#endif

#ifndef EV_USE_PORT
# define EV_USE_PORT 0
#endif

#ifndef EV_USE_INOTIFY
# if __linux && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 4))
#  define EV_USE_INOTIFY EV_FEATURE_OS
# else
#  define EV_USE_INOTIFY 0
# endif
#endif

#ifndef EV_PID_HASHSIZE
# define EV_PID_HASHSIZE EV_FEATURE_DATA ? 16 : 1
#endif

#ifndef EV_INOTIFY_HASHSIZE
# define EV_INOTIFY_HASHSIZE EV_FEATURE_DATA ? 16 : 1
#endif

#ifndef EV_USE_EVENTFD
# if __linux && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 7))
#  define EV_USE_EVENTFD EV_FEATURE_OS
# else
#  define EV_USE_EVENTFD 0
# endif
#endif

#ifndef EV_USE_SIGNALFD
# if __linux && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 7))
#  define EV_USE_SIGNALFD EV_FEATURE_OS
# else
#  define EV_USE_SIGNALFD 0
# endif
#endif

#if 0 /* debugging */
# define EV_VERIFY 3
# define EV_USE_4HEAP 1
# define EV_HEAP_CACHE_AT 1
#endif

#ifndef EV_VERIFY
# define EV_VERIFY (EV_FEATURE_API ? 1 : 0)
#endif

#ifndef EV_USE_4HEAP
# define EV_USE_4HEAP EV_FEATURE_DATA
#endif

#ifndef EV_HEAP_CACHE_AT
# define EV_HEAP_CACHE_AT EV_FEATURE_DATA
#endif

#ifdef ANDROID
/* supposedly, android doesn't typedef fd_mask */
# undef EV_USE_SELECT
# define EV_USE_SELECT 0
/* supposedly, we need to include syscall.h, not sys/syscall.h, so just disable */
# undef EV_USE_CLOCK_SYSCALL
# define EV_USE_CLOCK_SYSCALL 0
#endif

/* aix's poll.h seems to cause lots of trouble */
#ifdef _AIX
/* AIX has a completely broken poll.h header */
# undef EV_USE_POLL
# define EV_USE_POLL 0
#endif

/* on linux, we can use a (slow) syscall to avoid a dependency on pthread, */
/* which makes programs even slower. might work on other unices, too. */
#if EV_USE_CLOCK_SYSCALL
# include <sys/syscall.h>
# ifdef SYS_clock_gettime
#  define clock_gettime(id, ts) syscall (SYS_clock_gettime, (id), (ts))
#  undef EV_USE_MONOTONIC
#  define EV_USE_MONOTONIC 1
# else
#  undef EV_USE_CLOCK_SYSCALL
#  define EV_USE_CLOCK_SYSCALL 0
# endif
#endif

/* this block fixes any misconfiguration where we know we run into trouble otherwise */

#ifndef CLOCK_MONOTONIC
# undef EV_USE_MONOTONIC
# define EV_USE_MONOTONIC 0
#endif

#ifndef CLOCK_REALTIME
# undef EV_USE_REALTIME
# define EV_USE_REALTIME 0
#endif

#if !EV_STAT_ENABLE
# undef EV_USE_INOTIFY
# define EV_USE_INOTIFY 0
#endif

#if !EV_USE_NANOSLEEP
/* hp-ux has it in sys/time.h, which we unconditionally include above */
# if !defined _WIN32 && !defined __hpux
#  include <sys/select.h>
# endif
#endif

#if EV_USE_INOTIFY
# include <sys/statfs.h>
# include <sys/inotify.h>
/* some very old inotify.h headers don't have IN_DONT_FOLLOW */
# ifndef IN_DONT_FOLLOW
#  undef EV_USE_INOTIFY
#  define EV_USE_INOTIFY 0
# endif
#endif

#if EV_USE_EVENTFD
/* our minimum requirement is glibc 2.7 which has the stub, but not the header */
# include <stdint.h>
# ifndef EFD_NONBLOCK
#  define EFD_NONBLOCK O_NONBLOCK
# endif
# ifndef EFD_CLOEXEC
#  ifdef O_CLOEXEC
#   define EFD_CLOEXEC O_CLOEXEC
#  else
#   define EFD_CLOEXEC 02000000
#  endif
# endif
EV_CPP(extern "C") int (eventfd) (unsigned int initval, int flags);
#endif

#if EV_USE_SIGNALFD
/* our minimum requirement is glibc 2.7 which has the stub, but not the header */
# include <stdint.h>
# ifndef SFD_NONBLOCK
#  define SFD_NONBLOCK O_NONBLOCK
# endif
# ifndef SFD_CLOEXEC
#  ifdef O_CLOEXEC
#   define SFD_CLOEXEC O_CLOEXEC
#  else
#   define SFD_CLOEXEC 02000000
#  endif
# endif
EV_CPP (extern "C") int signalfd (int fd, const sigset_t *mask, int flags);

struct signalfd_siginfo
{
    uint32_t ssi_signo;
    char pad[128 - sizeof (uint32_t)];
};
#endif

/**/

#if EV_VERIFY >= 3
# define EV_FREQUENT_CHECK ev_verify (loop)
#else
# define EV_FREQUENT_CHECK do { } while (0)
#endif

/*
 * This is used to work around floating point rounding problems.
 * This value is good at least till the year 4000.
 */
#define MIN_INTERVAL  0.0001220703125 /* 1/2**13, good till 4000 */
/*#define MIN_INTERVAL  0.00000095367431640625 / * 1/2**20, good till 2200 */

#define MIN_TIMEJUMP  1. /* minimum timejump that gets detected (if monotonic clock available) */
#define MAX_BLOCKTIME 59.743 /* never wait longer than this time (to detect time jumps) */

#define EV_TV_SET(tv,t) do { tv.tv_sec = (long)t; tv.tv_usec = (long)((t - tv.tv_sec) * 1e6); } while (0)
#define EV_TS_SET(ts,t) do { ts.tv_sec = (long)t; ts.tv_nsec = (long)((t - ts.tv_sec) * 1e9); } while (0)

/* the following is ecb.h embedded into libev - use update_ev_c to update from an external copy */
/* ECB.H BEGIN */
/*
 * libecb - http://software.schmorp.de/pkg/libecb
 *
 * Copyright (©) 2009-2012 Marc Alexander Lehmann <libecb@schmorp.de>
 * Copyright (©) 2011 Emanuele Giaquinta
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 *
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ECB_H
#define ECB_H

/* 16 bits major, 16 bits minor */
#define ECB_VERSION 0x00010003

#ifdef _WIN32
typedef   signed char   int8_t;
typedef unsigned char  uint8_t;
typedef   signed short  int16_t;
typedef unsigned short uint16_t;
typedef   signed int    int32_t;
typedef unsigned int   uint32_t;
#if __GNUC__
typedef   signed long long int64_t;
typedef unsigned long long uint64_t;
#else /* _MSC_VER || __BORLANDC__ */
typedef   signed __int64   int64_t;
typedef unsigned __int64   uint64_t;
#endif
#ifdef _WIN64
#define ECB_PTRSIZE 8
typedef uint64_t uintptr_t;
typedef  int64_t  intptr_t;
#else
#define ECB_PTRSIZE 4
typedef uint32_t uintptr_t;
typedef  int32_t  intptr_t;
#endif
#else
#include <inttypes.h>
#if UINTMAX_MAX > 0xffffffffU
#define ECB_PTRSIZE 8
#else
#define ECB_PTRSIZE 4
#endif
#endif

/* work around x32 idiocy by defining proper macros */
#if __x86_64 || _M_AMD64
#if __ILP32
#define ECB_AMD64_X32 1
#else
#define ECB_AMD64 1
#endif
#endif

/* many compilers define _GNUC_ to some versions but then only implement
 * what their idiot authors think are the "more important" extensions,
 * causing enormous grief in return for some better fake benchmark numbers.
 * or so.
 * we try to detect these and simply assume they are not gcc - if they have
 * an issue with that they should have done it right in the first place.
 */
#ifndef ECB_GCC_VERSION
#if !defined __GNUC_MINOR__ || defined __INTEL_COMPILER || defined __SUNPRO_C || defined __SUNPRO_CC || defined __llvm__ || defined __clang__
#define ECB_GCC_VERSION(major,minor) 0
#else
#define ECB_GCC_VERSION(major,minor) (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif
#endif

#define ECB_C     (__STDC__+0) /* this assumes that __STDC__ is either empty or a number */
#define ECB_C99   (__STDC_VERSION__ >= 199901L)
#define ECB_C11   (__STDC_VERSION__ >= 201112L)
#define ECB_CPP   (__cplusplus+0)
#define ECB_CPP11 (__cplusplus >= 201103L)

#if ECB_CPP
#define ECB_EXTERN_C extern "C"
#define ECB_EXTERN_C_BEG ECB_EXTERN_C {
#define ECB_EXTERN_C_END }
#else
#define ECB_EXTERN_C extern
#define ECB_EXTERN_C_BEG
#define ECB_EXTERN_C_END
#endif

/*****************************************************************************/

/* ECB_NO_THREADS - ecb is not used by multiple threads, ever */
/* ECB_NO_SMP     - ecb might be used in multiple threads, but only on a single cpu */

#if ECB_NO_THREADS
#define ECB_NO_SMP 1
#endif

#if ECB_NO_SMP
#define ECB_MEMORY_FENCE do { } while (0)
#endif

#ifndef ECB_MEMORY_FENCE
#if ECB_GCC_VERSION(2,5) || defined __INTEL_COMPILER || (__llvm__ && __GNUC__) || __SUNPRO_C >= 0x5110 || __SUNPRO_CC >= 0x5110
#if __i386 || __i386__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("lock; orb $0, -1(%%esp)" : : : "memory")
#define ECB_MEMORY_FENCE_ACQUIRE __asm__ __volatile__ (""                        : : : "memory")
#define ECB_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("")
#elif __amd64 || __amd64__ || __x86_64 || __x86_64__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("mfence"   : : : "memory")
#define ECB_MEMORY_FENCE_ACQUIRE __asm__ __volatile__ (""         : : : "memory")
#define ECB_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("")
#elif __powerpc__ || __ppc__ || __powerpc64__ || __ppc64__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("sync"     : : : "memory")
#elif defined __ARM_ARCH_6__  || defined __ARM_ARCH_6J__  \
       || defined __ARM_ARCH_6K__ || defined __ARM_ARCH_6ZK__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("mcr p15,0,%0,c7,c10,5" : : "r" (0) : "memory")
#elif defined __ARM_ARCH_7__  || defined __ARM_ARCH_7A__  \
       || defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7R__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("dmb"      : : : "memory")
#elif __sparc || __sparc__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("membar #LoadStore | #LoadLoad | #StoreStore | #StoreLoad" : : : "memory")
#define ECB_MEMORY_FENCE_ACQUIRE __asm__ __volatile__ ("membar #LoadStore | #LoadLoad"                            : : : "memory")
#define ECB_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("membar #LoadStore             | #StoreStore")
#elif defined __s390__ || defined __s390x__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("bcr 15,0" : : : "memory")
#elif defined __mips__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("sync"     : : : "memory")
#elif defined __alpha__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("mb"       : : : "memory")
#elif defined __hppa__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ (""         : : : "memory")
#define ECB_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("")
#elif defined __ia64__
#define ECB_MEMORY_FENCE         __asm__ __volatile__ ("mf"       : : : "memory")
#endif
#endif
#endif

#ifndef ECB_MEMORY_FENCE
#if ECB_GCC_VERSION(4,7)
/* see comment below (stdatomic.h) about the C11 memory model. */
#define ECB_MEMORY_FENCE         __atomic_thread_fence (__ATOMIC_SEQ_CST)

/* The __has_feature syntax from clang is so misdesigned that we cannot use it
 * without risking compile time errors with other compilers. We *could*
 * define our own ecb_clang_has_feature, but I just can't be bothered to work
 * around this shit time and again.
 * #elif defined __clang && __has_feature (cxx_atomic)
 *   // see comment below (stdatomic.h) about the C11 memory model.
 *   #define ECB_MEMORY_FENCE         __c11_atomic_thread_fence (__ATOMIC_SEQ_CST)
 */

#elif ECB_GCC_VERSION(4,4) || defined __INTEL_COMPILER || defined __clang__
#define ECB_MEMORY_FENCE         __sync_synchronize ()
#elif _MSC_VER >= 1400 /* VC++ 2005 */
#pragma intrinsic(_ReadBarrier,_WriteBarrier,_ReadWriteBarrier)
#define ECB_MEMORY_FENCE         _ReadWriteBarrier ()
#define ECB_MEMORY_FENCE_ACQUIRE _ReadWriteBarrier () /* according to msdn, _ReadBarrier is not a load fence */
#define ECB_MEMORY_FENCE_RELEASE _WriteBarrier ()
#elif defined _WIN32
#include <WinNT.h>
#define ECB_MEMORY_FENCE         MemoryBarrier () /* actually just xchg on x86... scary */
#elif __SUNPRO_C >= 0x5110 || __SUNPRO_CC >= 0x5110
#include <mbarrier.h>
#define ECB_MEMORY_FENCE         __machine_rw_barrier ()
#define ECB_MEMORY_FENCE_ACQUIRE __machine_r_barrier  ()
#define ECB_MEMORY_FENCE_RELEASE __machine_w_barrier  ()
#elif __xlC__
#define ECB_MEMORY_FENCE         __sync ()
#endif
#endif

#ifndef ECB_MEMORY_FENCE
#if ECB_C11 && !defined __STDC_NO_ATOMICS__
/* we assume that these memory fences work on all variables/all memory accesses, */
/* not just C11 atomics and atomic accesses */
#include <stdatomic.h>
/* Unfortunately, neither gcc 4.7 nor clang 3.1 generate any instructions for */
/* any fence other than seq_cst, which isn't very efficient for us. */
/* Why that is, we don't know - either the C11 memory model is quite useless */
/* for most usages, or gcc and clang have a bug */
/* I *currently* lean towards the latter, and inefficiently implement */
/* all three of ecb's fences as a seq_cst fence */
#define ECB_MEMORY_FENCE         atomic_thread_fence (memory_order_seq_cst)
#endif
#endif

#ifndef ECB_MEMORY_FENCE
#if !ECB_AVOID_PTHREADS
/*
 * if you get undefined symbol references to pthread_mutex_lock,
 * or failure to find pthread.h, then you should implement
 * the ECB_MEMORY_FENCE operations for your cpu/compiler
 * OR provide pthread.h and link against the posix thread library
 * of your system.
 */
#include <pthread.h>
#define ECB_NEEDS_PTHREADS 1
#define ECB_MEMORY_FENCE_NEEDS_PTHREADS 1

static pthread_mutex_t ecb_mf_lock = PTHREAD_MUTEX_INITIALIZER;
#define ECB_MEMORY_FENCE do { pthread_mutex_lock (&ecb_mf_lock); pthread_mutex_unlock (&ecb_mf_lock); } while (0)
#endif
#endif

#if !defined ECB_MEMORY_FENCE_ACQUIRE && defined ECB_MEMORY_FENCE
#define ECB_MEMORY_FENCE_ACQUIRE ECB_MEMORY_FENCE
#endif

#if !defined ECB_MEMORY_FENCE_RELEASE && defined ECB_MEMORY_FENCE
#define ECB_MEMORY_FENCE_RELEASE ECB_MEMORY_FENCE
#endif

/*****************************************************************************/

#if __cplusplus
#define ecb_inline static inline
#elif ECB_GCC_VERSION(2,5)
#define ecb_inline static __inline__
#elif ECB_C99
#define ecb_inline static inline
#else
#define ecb_inline static
#endif

#if ECB_GCC_VERSION(3,3)
#define ecb_restrict __restrict__
#elif ECB_C99
#define ecb_restrict restrict
#else
#define ecb_restrict
#endif

typedef int ecb_bool;

#define ECB_CONCAT_(a, b) a ## b
#define ECB_CONCAT(a, b) ECB_CONCAT_(a, b)
#define ECB_STRINGIFY_(a) # a
#define ECB_STRINGIFY(a) ECB_STRINGIFY_(a)

#define ecb_function_ ecb_inline

#if ECB_GCC_VERSION(3,1)
#define ecb_attribute(attrlist)        __attribute__(attrlist)
#define ecb_is_constant(expr)          __builtin_constant_p (expr)
#define ecb_expect(expr,value)         __builtin_expect ((expr),(value))
#define ecb_prefetch(addr,rw,locality) __builtin_prefetch (addr, rw, locality)
#else
#define ecb_attribute(attrlist)
#define ecb_is_constant(expr)          0
#define ecb_expect(expr,value)         (expr)
#define ecb_prefetch(addr,rw,locality)
#endif

/* no emulation for ecb_decltype */
#if ECB_GCC_VERSION(4,5)
#define ecb_decltype(x) __decltype(x)
#elif ECB_GCC_VERSION(3,0)
#define ecb_decltype(x) __typeof(x)
#endif

#define ecb_noinline   ecb_attribute ((__noinline__))
#define ecb_unused     ecb_attribute ((__unused__))
#define ecb_const      ecb_attribute ((__const__))
#define ecb_pure       ecb_attribute ((__pure__))

#if ECB_C11
#define ecb_noreturn   _Noreturn
#else
#define ecb_noreturn   ecb_attribute ((__noreturn__))
#endif

#if ECB_GCC_VERSION(4,3)
#define ecb_artificial ecb_attribute ((__artificial__))
#define ecb_hot        ecb_attribute ((__hot__))
#define ecb_cold       ecb_attribute ((__cold__))
#else
#define ecb_artificial
#define ecb_hot
#define ecb_cold
#endif

/* put around conditional expressions if you are very sure that the  */
/* expression is mostly true or mostly false. note that these return */
/* booleans, not the expression.                                     */
#define ecb_expect_false(expr) ecb_expect (!!(expr), 0)
#define ecb_expect_true(expr)  ecb_expect (!!(expr), 1)
/* for compatibility to the rest of the world */
#define ecb_likely(expr)   ecb_expect_true  (expr)
#define ecb_unlikely(expr) ecb_expect_false (expr)

/* count trailing zero bits and count # of one bits */
#if ECB_GCC_VERSION(3,4)
/* we assume int == 32 bit, long == 32 or 64 bit and long long == 64 bit */
#define ecb_ld32(x)      (__builtin_clz      (x) ^ 31)
#define ecb_ld64(x)      (__builtin_clzll    (x) ^ 63)
#define ecb_ctz32(x)      __builtin_ctz      (x)
#define ecb_ctz64(x)      __builtin_ctzll    (x)
#define ecb_popcount32(x) __builtin_popcount (x)
/* no popcountll */
#else
ecb_function_ int ecb_ctz32 (uint32_t x) ecb_const;
ecb_function_ int
ecb_ctz32 (uint32_t x)
{
    int r = 0;

    x &= ~x + 1; /* this isolates the lowest bit */

#if ECB_branchless_on_i386
    r += !!(x & 0xaaaaaaaa) << 0;
    r += !!(x & 0xcccccccc) << 1;
    r += !!(x & 0xf0f0f0f0) << 2;
    r += !!(x & 0xff00ff00) << 3;
    r += !!(x & 0xffff0000) << 4;
#else
    if (x & 0xaaaaaaaa) r +=  1;
    if (x & 0xcccccccc) r +=  2;
    if (x & 0xf0f0f0f0) r +=  4;
    if (x & 0xff00ff00) r +=  8;
    if (x & 0xffff0000) r += 16;
#endif

    return r;
}

ecb_function_ int ecb_ctz64 (uint64_t x) ecb_const;
ecb_function_ int
ecb_ctz64 (uint64_t x)
{
    int shift = x & 0xffffffffU ? 0 : 32;
    return ecb_ctz32 (x >> shift) + shift;
}

ecb_function_ int ecb_popcount32 (uint32_t x) ecb_const;
ecb_function_ int
ecb_popcount32 (uint32_t x)
{
    x -=  (x >> 1) & 0x55555555;
    x  = ((x >> 2) & 0x33333333) + (x & 0x33333333);
    x  = ((x >> 4) + x) & 0x0f0f0f0f;
    x *= 0x01010101;

    return x >> 24;
}

ecb_function_ int ecb_ld32 (uint32_t x) ecb_const;
ecb_function_ int ecb_ld32 (uint32_t x)
{
    int r = 0;

    if (x >> 16) {
        x >>= 16;
        r += 16;
    }
    if (x >>  8) {
        x >>=  8;
        r +=  8;
    }
    if (x >>  4) {
        x >>=  4;
        r +=  4;
    }
    if (x >>  2) {
        x >>=  2;
        r +=  2;
    }
    if (x >>  1) {
        r +=  1;
    }

    return r;
}

ecb_function_ int ecb_ld64 (uint64_t x) ecb_const;
ecb_function_ int ecb_ld64 (uint64_t x)
{
    int r = 0;

    if (x >> 32) {
        x >>= 32;
        r += 32;
    }

    return r + ecb_ld32 (x);
}
#endif

ecb_function_ ecb_bool ecb_is_pot32 (uint32_t x) ecb_const;
ecb_function_ ecb_bool ecb_is_pot32 (uint32_t x) {
    return !(x & (x - 1));
}
ecb_function_ ecb_bool ecb_is_pot64 (uint64_t x) ecb_const;
ecb_function_ ecb_bool ecb_is_pot64 (uint64_t x) {
    return !(x & (x - 1));
}

ecb_function_ uint8_t  ecb_bitrev8  (uint8_t  x) ecb_const;
ecb_function_ uint8_t  ecb_bitrev8  (uint8_t  x)
{
    return (  (x * 0x0802U & 0x22110U)
              | (x * 0x8020U & 0x88440U)) * 0x10101U >> 16;
}

ecb_function_ uint16_t ecb_bitrev16 (uint16_t x) ecb_const;
ecb_function_ uint16_t ecb_bitrev16 (uint16_t x)
{
    x = ((x >>  1) &     0x5555) | ((x &     0x5555) <<  1);
    x = ((x >>  2) &     0x3333) | ((x &     0x3333) <<  2);
    x = ((x >>  4) &     0x0f0f) | ((x &     0x0f0f) <<  4);
    x = ( x >>  8              ) | ( x               <<  8);

    return x;
}

ecb_function_ uint32_t ecb_bitrev32 (uint32_t x) ecb_const;
ecb_function_ uint32_t ecb_bitrev32 (uint32_t x)
{
    x = ((x >>  1) & 0x55555555) | ((x & 0x55555555) <<  1);
    x = ((x >>  2) & 0x33333333) | ((x & 0x33333333) <<  2);
    x = ((x >>  4) & 0x0f0f0f0f) | ((x & 0x0f0f0f0f) <<  4);
    x = ((x >>  8) & 0x00ff00ff) | ((x & 0x00ff00ff) <<  8);
    x = ( x >> 16              ) | ( x               << 16);

    return x;
}

/* popcount64 is only available on 64 bit cpus as gcc builtin */
/* so for this version we are lazy */
ecb_function_ int ecb_popcount64 (uint64_t x) ecb_const;
ecb_function_ int
ecb_popcount64 (uint64_t x)
{
    return ecb_popcount32 (x) + ecb_popcount32 (x >> 32);
}

ecb_inline uint8_t  ecb_rotl8  (uint8_t  x, unsigned int count) ecb_const;
ecb_inline uint8_t  ecb_rotr8  (uint8_t  x, unsigned int count) ecb_const;
ecb_inline uint16_t ecb_rotl16 (uint16_t x, unsigned int count) ecb_const;
ecb_inline uint16_t ecb_rotr16 (uint16_t x, unsigned int count) ecb_const;
ecb_inline uint32_t ecb_rotl32 (uint32_t x, unsigned int count) ecb_const;
ecb_inline uint32_t ecb_rotr32 (uint32_t x, unsigned int count) ecb_const;
ecb_inline uint64_t ecb_rotl64 (uint64_t x, unsigned int count) ecb_const;
ecb_inline uint64_t ecb_rotr64 (uint64_t x, unsigned int count) ecb_const;

ecb_inline uint8_t  ecb_rotl8  (uint8_t  x, unsigned int count) {
    return (x >> ( 8 - count)) | (x << count);
}
ecb_inline uint8_t  ecb_rotr8  (uint8_t  x, unsigned int count) {
    return (x << ( 8 - count)) | (x >> count);
}
ecb_inline uint16_t ecb_rotl16 (uint16_t x, unsigned int count) {
    return (x >> (16 - count)) | (x << count);
}
ecb_inline uint16_t ecb_rotr16 (uint16_t x, unsigned int count) {
    return (x << (16 - count)) | (x >> count);
}
ecb_inline uint32_t ecb_rotl32 (uint32_t x, unsigned int count) {
    return (x >> (32 - count)) | (x << count);
}
ecb_inline uint32_t ecb_rotr32 (uint32_t x, unsigned int count) {
    return (x << (32 - count)) | (x >> count);
}
ecb_inline uint64_t ecb_rotl64 (uint64_t x, unsigned int count) {
    return (x >> (64 - count)) | (x << count);
}
ecb_inline uint64_t ecb_rotr64 (uint64_t x, unsigned int count) {
    return (x << (64 - count)) | (x >> count);
}

#if ECB_GCC_VERSION(4,3)
#define ecb_bswap16(x) (__builtin_bswap32 (x) >> 16)
#define ecb_bswap32(x)  __builtin_bswap32 (x)
#define ecb_bswap64(x)  __builtin_bswap64 (x)
#else
ecb_function_ uint16_t ecb_bswap16 (uint16_t x) ecb_const;
ecb_function_ uint16_t
ecb_bswap16 (uint16_t x)
{
    return ecb_rotl16 (x, 8);
}

ecb_function_ uint32_t ecb_bswap32 (uint32_t x) ecb_const;
ecb_function_ uint32_t
ecb_bswap32 (uint32_t x)
{
    return (((uint32_t)ecb_bswap16 (x)) << 16) | ecb_bswap16 (x >> 16);
}

ecb_function_ uint64_t ecb_bswap64 (uint64_t x) ecb_const;
ecb_function_ uint64_t
ecb_bswap64 (uint64_t x)
{
    return (((uint64_t)ecb_bswap32 (x)) << 32) | ecb_bswap32 (x >> 32);
}
#endif

#if ECB_GCC_VERSION(4,5)
#define ecb_unreachable() __builtin_unreachable ()
#else
/* this seems to work fine, but gcc always emits a warning for it :/ */
ecb_inline void ecb_unreachable (void) ecb_noreturn;
ecb_inline void ecb_unreachable (void) { }
#endif

/* try to tell the compiler that some condition is definitely true */
#define ecb_assume(cond) if (!(cond)) ecb_unreachable (); else 0

ecb_inline unsigned char ecb_byteorder_helper (void) ecb_const;
ecb_inline unsigned char
ecb_byteorder_helper (void)
{
    /* the union code still generates code under pressure in gcc, */
    /* but less than using pointers, and always seems to */
    /* successfully return a constant. */
    /* the reason why we have this horrible preprocessor mess */
    /* is to avoid it in all cases, at least on common architectures */
    /* or when using a recent enough gcc version (>= 4.6) */
#if __i386 || __i386__ || _M_X86 || __amd64 || __amd64__ || _M_X64
    return 0x44;
#elif __BYTE_ORDER__ && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return 0x44;
#elif __BYTE_ORDER__ && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return 0x11;
#else
    union
    {
        uint32_t i;
        uint8_t c;
    } u = { 0x11223344 };
    return u.c;
#endif
}

ecb_inline ecb_bool ecb_big_endian    (void) ecb_const;
ecb_inline ecb_bool ecb_big_endian    (void) {
    return ecb_byteorder_helper () == 0x11;
}
ecb_inline ecb_bool ecb_little_endian (void) ecb_const;
ecb_inline ecb_bool ecb_little_endian (void) {
    return ecb_byteorder_helper () == 0x44;
}

#if ECB_GCC_VERSION(3,0) || ECB_C99
#define ecb_mod(m,n) ((m) % (n) + ((m) % (n) < 0 ? (n) : 0))
#else
#define ecb_mod(m,n) ((m) < 0 ? ((n) - 1 - ((-1 - (m)) % (n))) : ((m) % (n)))
#endif

#if __cplusplus
template<typename T>
static inline T ecb_div_rd (T val, T div)
{
    return val < 0 ? - ((-val + div - 1) / div) : (val          ) / div;
}
template<typename T>
static inline T ecb_div_ru (T val, T div)
{
    return val < 0 ? - ((-val          ) / div) : (val + div - 1) / div;
}
#else
#define ecb_div_rd(val,div) ((val) < 0 ? - ((-(val) + (div) - 1) / (div)) : ((val)            ) / (div))
#define ecb_div_ru(val,div) ((val) < 0 ? - ((-(val)            ) / (div)) : ((val) + (div) - 1) / (div))
#endif

#if ecb_cplusplus_does_not_suck
/* does not work for local types (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2657.htm) */
template<typename T, int N>
static inline int ecb_array_length (const T (&arr)[N])
{
    return N;
}
#else
#define ecb_array_length(name) (sizeof (name) / sizeof (name [0]))
#endif

/*******************************************************************************/
/* floating point stuff, can be disabled by defining ECB_NO_LIBM */

/* basically, everything uses "ieee pure-endian" floating point numbers */
/* the only noteworthy exception is ancient armle, which uses order 43218765 */
#if 0 \
    || __i386 || __i386__ \
    || __amd64 || __amd64__ || __x86_64 || __x86_64__ \
    || __powerpc__ || __ppc__ || __powerpc64__ || __ppc64__ \
    || defined __arm__ && defined __ARM_EABI__ \
    || defined __s390__ || defined __s390x__ \
    || defined __mips__ \
    || defined __alpha__ \
    || defined __hppa__ \
    || defined __ia64__ \
    || defined _M_IX86 || defined _M_AMD64 || defined _M_IA64
#define ECB_STDFP 1
#include <string.h> /* for memcpy */
#else
#define ECB_STDFP 0
#include <math.h> /* for frexp*, ldexp* */
#endif

#ifndef ECB_NO_LIBM

/* convert a float to ieee single/binary32 */
ecb_function_ uint32_t ecb_float_to_binary32 (float x) ecb_const;
ecb_function_ uint32_t
ecb_float_to_binary32 (float x)
{
    uint32_t r;

#if ECB_STDFP
    memcpy (&r, &x, 4);
#else
    /* slow emulation, works for anything but -0 */
    uint32_t m;
    int e;

    if (x == 0e0f                    ) return 0x00000000U;
    if (x > +3.40282346638528860e+38f) return 0x7f800000U;
    if (x < -3.40282346638528860e+38f) return 0xff800000U;
    if (x != x                       ) return 0x7fbfffffU;

    m = frexpf (x, &e) * 0x1000000U;

    r = m & 0x80000000U;

    if (r)
        m = -m;

    if (e <= -126)
    {
        m &= 0xffffffU;
        m >>= (-125 - e);
        e = -126;
    }

    r |= (e + 126) << 23;
    r |= m & 0x7fffffU;
#endif

    return r;
}

/* converts an ieee single/binary32 to a float */
ecb_function_ float ecb_binary32_to_float (uint32_t x) ecb_const;
ecb_function_ float
ecb_binary32_to_float (uint32_t x)
{
    float r;

#if ECB_STDFP
    memcpy (&r, &x, 4);
#else
    /* emulation, only works for normals and subnormals and +0 */
    int neg = x >> 31;
    int e = (x >> 23) & 0xffU;

    x &= 0x7fffffU;

    if (e)
        x |= 0x800000U;
    else
        e = 1;

    /* we distrust ldexpf a bit and do the 2**-24 scaling by an extra multiply */
    r = ldexpf (x * (0.5f / 0x800000U), e - 126);

    r = neg ? -r : r;
#endif

    return r;
}

/* convert a double to ieee double/binary64 */
ecb_function_ uint64_t ecb_double_to_binary64 (double x) ecb_const;
ecb_function_ uint64_t
ecb_double_to_binary64 (double x)
{
    uint64_t r;

#if ECB_STDFP
    memcpy (&r, &x, 8);
#else
    /* slow emulation, works for anything but -0 */
    uint64_t m;
    int e;

    if (x == 0e0                     ) return 0x0000000000000000U;
    if (x > +1.79769313486231470e+308) return 0x7ff0000000000000U;
    if (x < -1.79769313486231470e+308) return 0xfff0000000000000U;
    if (x != x                       ) return 0X7ff7ffffffffffffU;

    m = frexp (x, &e) * 0x20000000000000U;

    r = m & 0x8000000000000000;;

    if (r)
        m = -m;

    if (e <= -1022)
    {
        m &= 0x1fffffffffffffU;
        m >>= (-1021 - e);
        e = -1022;
    }

    r |= ((uint64_t)(e + 1022)) << 52;
    r |= m & 0xfffffffffffffU;
#endif

    return r;
}

/* converts an ieee double/binary64 to a double */
ecb_function_ double ecb_binary64_to_double (uint64_t x) ecb_const;
ecb_function_ double
ecb_binary64_to_double (uint64_t x)
{
    double r;

#if ECB_STDFP
    memcpy (&r, &x, 8);
#else
    /* emulation, only works for normals and subnormals and +0 */
    int neg = x >> 63;
    int e = (x >> 52) & 0x7ffU;

    x &= 0xfffffffffffffU;

    if (e)
        x |= 0x10000000000000U;
    else
        e = 1;

    /* we distrust ldexp a bit and do the 2**-53 scaling by an extra multiply */
    r = ldexp (x * (0.5 / 0x10000000000000U), e - 1022);

    r = neg ? -r : r;
#endif

    return r;
}

#endif

#endif

/* ECB.H END */

#if ECB_MEMORY_FENCE_NEEDS_PTHREADS
/* if your architecture doesn't need memory fences, e.g. because it is
 * single-cpu/core, or if you use libev in a project that doesn't use libev
 * from multiple threads, then you can define ECB_AVOID_PTHREADS when compiling
 * libev, in which cases the memory fences become nops.
 * alternatively, you can remove this #error and link against libpthread,
 * which will then provide the memory fences.
 */
# error "memory fences not defined for your architecture, please report"
#endif

#ifndef ECB_MEMORY_FENCE
# define ECB_MEMORY_FENCE do { } while (0)
# define ECB_MEMORY_FENCE_ACQUIRE ECB_MEMORY_FENCE
# define ECB_MEMORY_FENCE_RELEASE ECB_MEMORY_FENCE
#endif

#define expect_false(cond) ecb_expect_false (cond)
#define expect_true(cond)  ecb_expect_true  (cond)
#define noinline           ecb_noinline

#define inline_size        ecb_inline

#if EV_FEATURE_CODE
# define inline_speed      ecb_inline
#else
# define inline_speed      static noinline
#endif

#define NUMPRI (EV_MAXPRI - EV_MINPRI + 1)

#if EV_MINPRI == EV_MAXPRI
# define ABSPRI(w) (((ev_watcher*)w), 0)
#else
# define ABSPRI(w) (((ev_watcher*)w)->priority - EV_MINPRI)
#endif

#define EMPTY       /* required for microsofts broken pseudo-c compiler */
#define EMPTY2(a,b) /* used to suppress some warnings */

typedef ev_watcher_list *WL;
typedef ev_watcher_time *WT;

#define ev_active(w) ((ev_watcher*)(w))->active
#define ev_at(w) ((WT)(w))->at

#if EV_USE_REALTIME
/* sig_atomic_t is used to avoid per-thread variables or locking but still */
/* giving it a reasonably high chance of working on typical architectures */
static EV_ATOMIC_T have_realtime; /* did clock_gettime (CLOCK_REALTIME) work? */
#endif

#if EV_USE_MONOTONIC
static EV_ATOMIC_T have_monotonic; /* did clock_gettime (CLOCK_MONOTONIC) work? */
#endif

#ifndef EV_FD_TO_WIN32_HANDLE
# define EV_FD_TO_WIN32_HANDLE(fd) _get_osfhandle (fd)
#endif
#ifndef EV_WIN32_HANDLE_TO_FD
# define EV_WIN32_HANDLE_TO_FD(handle) _open_osfhandle (handle, 0)
#endif
#ifndef EV_WIN32_CLOSE_FD
# define EV_WIN32_CLOSE_FD(fd) close (fd)
#endif

#ifdef _WIN32
# include "ev_win32.c"
#endif

/*****************************************************************************/

/* define a suitable floor function (only used by periodics atm) */

#if EV_USE_FLOOR
# include <math.h>
# define ev_floor(v) floor (v)
#else

#include <float.h>

/* a floor() replacement function, should be independent of ev_tstamp type */
static ev_tstamp noinline
ev_floor (ev_tstamp v)
{
    /* the choice of shift factor is not terribly important */
#if FLT_RADIX != 2 /* assume FLT_RADIX == 10 */
    const ev_tstamp shift = sizeof (unsigned long) >= 8 ? 10000000000000000000. : 1000000000.;
#else
    const ev_tstamp shift = sizeof (unsigned long) >= 8 ? 18446744073709551616. : 4294967296.;
#endif

    /* argument too large for an unsigned long? */
    if (expect_false (v >= shift))
    {
        ev_tstamp f;

        if (v == v - 1.)
            return v; /* very large number */

        f = shift * ev_floor (v * (1. / shift));
        return f + ev_floor (v - f);
    }

    /* special treatment for negative args? */
    if (expect_false (v < 0.))
    {
        ev_tstamp f = -ev_floor (-v);

        return f - (f == v ? 0 : 1);
    }

    /* fits into an unsigned long */
    return (unsigned long)v;
}

#endif

/*****************************************************************************/

#ifdef __linux
# include <sys/utsname.h>
#endif

static unsigned int noinline ecb_cold
ev_linux_version (void)
{
#ifdef __linux
    unsigned int v = 0;
    struct utsname buf;
    int i;
    char *p = buf.release;

    if (uname (&buf))
        return 0;

    for (i = 3+1; --i; )
    {
        unsigned int c = 0;

        for (;;)
        {
            if (*p >= '0' && *p <= '9')
                c = c * 10 + *p++ - '0';
            else
            {
                p += *p == '.';
                break;
            }
        }

        v = (v << 8) | c;
    }

    return v;
#else
    return 0;
#endif
}

/*****************************************************************************/

#if EV_AVOID_STDIO
static void noinline ecb_cold
ev_printerr (const char *msg)
{
    write (STDERR_FILENO, msg, strlen (msg));
}
#endif

static void (*syserr_cb)(const char *msg) EV_THROW;

void ecb_cold
ev_set_syserr_cb (void (*cb)(const char *msg) EV_THROW) EV_THROW
{
    syserr_cb = cb;
}

static void noinline ecb_cold
ev_syserr (const char *msg)
{
    if (!msg)
        msg = "(libev) system error";

    if (syserr_cb)
        syserr_cb (msg);
    else
    {
#if EV_AVOID_STDIO
        ev_printerr (msg);
        ev_printerr (": ");
        ev_printerr (strerror (errno));
        ev_printerr ("\n");
#else
        perror (msg);
#endif
        abort ();
    }
}

static void *
ev_realloc_emul (void *ptr, long size) EV_THROW
{
    /* some systems, notably openbsd and darwin, fail to properly
     * implement realloc (x, 0) (as required by both ansi c-89 and
     * the single unix specification, so work around them here.
     * recently, also (at least) fedora and debian started breaking it,
     * despite documenting it otherwise.
     */

    if (size)
        return realloc (ptr, size);

    free (ptr);
    return 0;
}

static void *(*alloc)(void *ptr, long size) EV_THROW = ev_realloc_emul;

void ecb_cold
ev_set_allocator (void *(*cb)(void *ptr, long size) EV_THROW) EV_THROW
{
    alloc = cb;
}

inline_speed void *
ev_realloc (void *ptr, long size)
{
    ptr = alloc (ptr, size);

    if (!ptr && size)
    {
#if EV_AVOID_STDIO
        ev_printerr ("(libev) memory allocation failed, aborting.\n");
#else
        fprintf (stderr, "(libev) cannot allocate %ld bytes, aborting.", size);
#endif
        abort ();
    }

    return ptr;
}

#define ev_malloc(size) ev_realloc (0, (size))
#define ev_free(ptr)    ev_realloc ((ptr), 0)

/*****************************************************************************/

/* set in reify when reification needed */
#define EV_ANFD_REIFY 1

/* file descriptor info structure */
typedef struct
{
    WL head;
    unsigned char events; /* the events watched for */
    unsigned char reify;  /* flag set when this ANFD needs reification (EV_ANFD_REIFY, EV__IOFDSET) */
    unsigned char emask;  /* the epoll backend stores the actual kernel mask in here */
    unsigned char unused;
#if EV_USE_EPOLL
    unsigned int egen;    /* generation counter to counter epoll bugs */
#endif
#if EV_SELECT_IS_WINSOCKET || EV_USE_IOCP
    SOCKET handle;
#endif
#if EV_USE_IOCP
    OVERLAPPED or, ow;
#endif
} ANFD;

/* stores the pending event set for a given watcher */
typedef struct
{
    ev_watcher* w;
    int events; /* the pending event set for the given watcher */
} ANPENDING;

/* hash table entry per inotify-id */
typedef struct
{
    WL head;
} ANFS;

/* Heap Entry */
#if EV_HEAP_CACHE_AT
/* a heap element */
typedef struct {
    ev_tstamp at;
    WT w;
} ANHE;

#define ANHE_w(he)        (he).w     /* access watcher, read-write */
#define ANHE_at(he)       (he).at    /* access cached at, read-only */
#define ANHE_at_cache(he) (he).at = (he).w->at /* update at from watcher */
#else
/* a heap element */
typedef WT ANHE;

#define ANHE_w(he)        (he)
#define ANHE_at(he)       (he)->at
#define ANHE_at_cache(he)
#endif

struct ev_loop
{
    ev_tstamp ev_rt_now;
    ev_tstamp now_floor; /* last time we refreshed rt_time */
    ev_tstamp mn_now;    /* monotonic clock "now" */
    ev_tstamp rtmn_diff; /* difference realtime - monotonic time */

    /* for reverse feeding of events */
    ev_watcher**rfeeds;
    int rfeedmax;
    int rfeedcnt;

    ANPENDING *pendings [NUMPRI];
    int pendingmax [NUMPRI];
    int pendingcnt [NUMPRI];
    int pendingpri; /* highest priority currently pending */
    ev_prepare pending_w; /* dummy pending watcher */

    ev_tstamp io_blocktime;
    ev_tstamp timeout_blocktime;

    int backend;
    int activecnt; /* total number of active events ("refcount") */
    EV_ATOMIC_T loop_done;  /* signal by ev_break */

    int backend_fd;
    ev_tstamp backend_mintime; /* assumed typical timer resolution */
    void (*backend_modify)(struct ev_loop *loop, int fd, int oev, int nev);
    void (*backend_poll)(struct ev_loop *loop, ev_tstamp timeout);

    ANFD *anfds;
    int anfdmax;

    int evpipe[2];
    ev_io pipe_w;
    EV_ATOMIC_T pipe_write_wanted;
    EV_ATOMIC_T pipe_write_skipped;

    pid_t curpid;

    char postfork;  /* true if we need to recreate kernel state after fork */

    void *vec_ri;
    void *vec_ro;
    void *vec_wi;
    void *vec_wo;
#if defined(_WIN32) || EV_GENWRAP
    void *vec_eo;
#endif
    int vec_max;

    struct pollfd *polls;
    int pollmax;
    int pollcnt;
    int *pollidxs; /* maps fds into structure indices */
    int pollidxmax;

    struct epoll_event *epoll_events;
    int epoll_eventmax;
    int *epoll_eperms;
    int epoll_epermcnt;
    int epoll_epermmax;

#if EV_USE_KQUEUE || EV_GENWRAP
    pid_t kqueue_fd_pid;
    struct kevent *kqueue_changes;
    int kqueue_changemax;
    int kqueue_changecnt;
    struct kevent *kqueue_events;
    int kqueue_eventmax;
#endif

    struct port_event *port_events;
    int port_eventmax;

#if EV_USE_IOCP || EV_GENWRAP
    HANDLE iocp;
#endif

    int *fdchanges;
    int fdchangemax;
    int fdchangecnt;

    ANHE *timers;
    int timermax;
    int timercnt;

    ANHE *periodics;
    int periodicmax;
    int periodiccnt;

    ev_idle **idles [NUMPRI];
    int idlemax [NUMPRI];
    int idlecnt [NUMPRI];
    int idleall; /* total number */

    struct ev_prepare **prepares;
    int preparemax;
    int preparecnt;

    struct ev_check **checks;
    int checkmax;
    int checkcnt;

    struct ev_fork **forks;
    int forkmax;
    int forkcnt;

    struct ev_cleanup **cleanups;
    int cleanupmax;
    int cleanupcnt;

    EV_ATOMIC_T async_pending;
    struct ev_async **asyncs;
    int asyncmax;
    int asynccnt;

    int fs_fd;
    ev_io fs_w;
    char fs_2625; /* whether we are running in linux 2.6.25 or newer */
    ANFS fs_hash [EV_INOTIFY_HASHSIZE];

    EV_ATOMIC_T sig_pending;
    int sigfd;
    ev_io sigfd_w;
    sigset_t sigfd_set;

    unsigned int origflags; /* original loop flags */

    unsigned int loop_count; /* total number of loop iterations/blocks */
    unsigned int loop_depth; /* #ev_run enters - #ev_run leaves */

    void *userdata;
    void (*release_cb)(struct ev_loop *loop) EV_THROW;
    void (*acquire_cb)(struct ev_loop *loop) EV_THROW;
    void (*invoke_cb) (struct ev_loop *loop);
};

static struct ev_loop default_loop_struct;
extern struct ev_loop *ev_default_loop_ptr; /* needs to be initialised to make it a definition despite extern */
struct ev_loop *ev_default_loop_ptr = 0;

# define EV_RELEASE_CB if (expect_false (loop->release_cb)) loop->release_cb (loop)
# define EV_ACQUIRE_CB if (expect_false (loop->acquire_cb)) loop->acquire_cb (loop)
# define EV_INVOKE_PENDING loop->invoke_cb (loop)

#define EVBREAK_RECURSE 0x80

/*****************************************************************************/

#ifndef EV_HAVE_EV_TIME
ev_tstamp
ev_time (void) EV_THROW
{
#if EV_USE_REALTIME
    if (expect_true (have_realtime))
    {
        struct timespec ts;
        clock_gettime (CLOCK_REALTIME, &ts);
        return ts.tv_sec + ts.tv_nsec * 1e-9;
    }
#endif

    struct timeval tv;
    gettimeofday (&tv, 0);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif

inline_size ev_tstamp
get_clock (void)
{
#if EV_USE_MONOTONIC
    if (expect_true (have_monotonic))
    {
        struct timespec ts;
        clock_gettime (CLOCK_MONOTONIC, &ts);
        return ts.tv_sec + ts.tv_nsec * 1e-9;
    }
#endif

    return ev_time ();
}

ev_tstamp
ev_now (struct ev_loop *loop) EV_THROW
{
    return loop->ev_rt_now;
}

void
ev_sleep (ev_tstamp delay) EV_THROW
{
    if (delay > 0.)
    {
#if EV_USE_NANOSLEEP
        struct timespec ts;

        EV_TS_SET (ts, delay);
        nanosleep (&ts, 0);
#elif defined _WIN32
        Sleep ((unsigned long)(delay * 1e3));
#else
        struct timeval tv;

        /* here we rely on sys/time.h + sys/types.h + unistd.h providing select */
        /* something not guaranteed by newer posix versions, but guaranteed */
        /* by older ones */
        EV_TV_SET (tv, delay);
        select (0, 0, 0, 0, &tv);
#endif
    }
}

/*****************************************************************************/

#define MALLOC_ROUND 4096 /* prefer to allocate in chunks of this size, must be 2**n and >> 4 longs */

/* find a suitable new size for the given array, */
/* hopefully by rounding to a nice-to-malloc size */
inline_size int
array_nextsize (int elem, int cur, int cnt)
{
    int ncur = cur + 1;

    do
        ncur <<= 1;
    while (cnt > ncur);

    /* if size is large, round to MALLOC_ROUND - 4 * longs to accommodate malloc overhead */
    if (elem * ncur > MALLOC_ROUND - sizeof (void *) * 4)
    {
        ncur *= elem;
        ncur = (ncur + elem + (MALLOC_ROUND - 1) + sizeof (void *) * 4) & ~(MALLOC_ROUND - 1);
        ncur = ncur - sizeof (void *) * 4;
        ncur /= elem;
    }

    return ncur;
}

static void * noinline ecb_cold
array_realloc (int elem, void *base, int *cur, int cnt)
{
    *cur = array_nextsize (elem, *cur, cnt);
    return ev_realloc (base, elem * *cur);
}

#define array_init_zero(base,count)	\
  memset ((void *)(base), 0, sizeof (*(base)) * (count))

#define array_needsize(type,base,cur,cnt,init)			\
  if (expect_false ((cnt) > (cur)))				\
    {								\
      int ecb_unused ocur_ = (cur);					\
      (base) = (type *)array_realloc				\
         (sizeof (type), (base), &(cur), (cnt));		\
      init ((base) + (ocur_), (cur) - ocur_);			\
    }

#if 0
#define array_slim(type,stem)					\
  if (stem ## max < array_roundsize (stem ## cnt >> 2))		\
    {								\
      stem ## max = array_roundsize (stem ## cnt >> 1);		\
      base = (type *)ev_realloc (base, sizeof (type) * (stem ## max));\
      fprintf (stderr, "slimmed down " # stem " to %d\n", stem ## max);/*D*/\
    }
#endif

#define array_free(stem, idx) \
  ev_free (stem ## s idx); stem ## cnt idx = stem ## max idx = 0; stem ## s idx = 0

/*****************************************************************************/

/* dummy callback for pending events */
static void noinline
pendingcb (struct ev_loop *loop, ev_prepare *w, int revents)
{
}

void noinline
ev_feed_event (struct ev_loop *loop, void *w, int revents) EV_THROW
{
    ev_watcher* w_ = (ev_watcher*)w;
    int pri = ABSPRI (w_);

    if (expect_false (w_->pending))
        loop->pendings [pri][w_->pending - 1].events |= revents;
    else
    {
        w_->pending = ++loop->pendingcnt [pri];
        array_needsize (ANPENDING, loop->pendings [pri], loop->pendingmax [pri], w_->pending, EMPTY2);
        loop->pendings [pri][w_->pending - 1].w      = w_;
        loop->pendings [pri][w_->pending - 1].events = revents;
    }

    loop->pendingpri = NUMPRI - 1;
}

inline_speed void
feed_reverse (struct ev_loop *loop, ev_watcher* w)
{
    array_needsize (ev_watcher*, loop->rfeeds, loop->rfeedmax, loop->rfeedcnt + 1, EMPTY2);
    loop->rfeeds [loop->rfeedcnt++] = w;
}

inline_size void
feed_reverse_done (struct ev_loop *loop, int revents)
{
    do
        ev_feed_event (loop, loop->rfeeds [--loop->rfeedcnt], revents);
    while (loop->rfeedcnt);
}

inline_speed void
queue_events (struct ev_loop *loop, ev_watcher* *events, int eventcnt, int type)
{
    int i;

    for (i = 0; i < eventcnt; ++i)
        ev_feed_event (loop, events [i], type);
}

/*****************************************************************************/

inline_speed void
fd_event_nocheck (struct ev_loop *loop, int fd, int revents)
{
    ANFD *anfd = loop->anfds + fd;
    ev_io *w;

    for (w = (ev_io *)anfd->head; w; w = (ev_io *)((WL)w)->next)
    {
        int ev = w->events & revents;

        if (ev)
            ev_feed_event (loop, (ev_watcher*)w, ev);
    }
}

/* do not submit kernel events for fds that have reify set */
/* because that means they changed while we were polling for new events */
inline_speed void
fd_event (struct ev_loop *loop, int fd, int revents)
{
    ANFD *anfd = loop->anfds + fd;

    if (expect_true (!anfd->reify))
        fd_event_nocheck (loop, fd, revents);
}

void
ev_feed_fd_event (struct ev_loop *loop, int fd, int revents) EV_THROW
{
    if (fd >= 0 && fd < loop->anfdmax)
        fd_event_nocheck (loop, fd, revents);
}

/* make sure the external fd watch events are in-sync */
/* with the kernel/libev internal state */
inline_size void
fd_reify (struct ev_loop *loop)
{
    int i;

#if EV_SELECT_IS_WINSOCKET || EV_USE_IOCP
    for (i = 0; i < fdchangecnt; ++i)
    {
        int fd = fdchanges [i];
        ANFD *anfd = anfds + fd;

        if (anfd->reify & EV__IOFDSET && anfd->head)
        {
            SOCKET handle = EV_FD_TO_WIN32_HANDLE (fd);

            if (handle != anfd->handle)
            {
                unsigned long arg;

                assert (("libev: only socket fds supported in this configuration", ioctlsocket (handle, FIONREAD, &arg) == 0));

                /* handle changed, but fd didn't - we need to do it in two steps */
                backend_modify (loop, fd, anfd->events, 0);
                anfd->events = 0;
                anfd->handle = handle;
            }
        }
    }
#endif

    for (i = 0; i < loop->fdchangecnt; ++i)
    {
        int fd = loop->fdchanges [i];
        ANFD *anfd = loop->anfds + fd;
        ev_io *w;

        unsigned char o_events = anfd->events;
        unsigned char o_reify  = anfd->reify;

        anfd->reify  = 0;

        /*if (expect_true (o_reify & EV_ANFD_REIFY)) probably a deoptimisation */
        {
            anfd->events = 0;

            for (w = (ev_io *)anfd->head; w; w = (ev_io *)((WL)w)->next)
                anfd->events |= (unsigned char)w->events;

            if (o_events != anfd->events)
                o_reify = EV__IOFDSET; /* actually |= */
        }

        if (o_reify & EV__IOFDSET)
            loop->backend_modify (loop, fd, o_events, anfd->events);
    }

    loop->fdchangecnt = 0;
}

/* something about the given fd changed */
inline_size void
fd_change (struct ev_loop *loop, int fd, int flags)
{
    unsigned char reify = loop->anfds [fd].reify;
    loop->anfds [fd].reify |= flags;

    if (expect_true (!reify))
    {
        ++loop->fdchangecnt;
        array_needsize (int, loop->fdchanges, loop->fdchangemax, loop->fdchangecnt, EMPTY2);
        loop->fdchanges [loop->fdchangecnt - 1] = fd;
    }
}

/* the given fd is invalid/unusable, so make sure it doesn't hurt us anymore */
inline_speed void ecb_cold
fd_kill (struct ev_loop *loop, int fd)
{
    ev_io *w;

    while ((w = (ev_io *)loop->anfds [fd].head))
    {
        ev_io_stop (loop, w);
        ev_feed_event (loop, (ev_watcher*)w, EV_ERROR | EV_READ | EV_WRITE);
    }
}

/* check whether the given fd is actually valid, for error recovery */
inline_size int ecb_cold
fd_valid (int fd)
{
#ifdef _WIN32
    return EV_FD_TO_WIN32_HANDLE (fd) != -1;
#else
    return fcntl (fd, F_GETFD) != -1;
#endif
}

/* called on EBADF to verify fds */
static void noinline ecb_cold
fd_ebadf (struct ev_loop *loop)
{
    int fd;

    for (fd = 0; fd < loop->anfdmax; ++fd)
        if (loop->anfds [fd].events)
            if (!fd_valid (fd) && errno == EBADF)
                fd_kill (loop, fd);
}

/* called on ENOMEM in select/poll to kill some fds and retry */
static void noinline ecb_cold
fd_enomem (struct ev_loop *loop)
{
    int fd;

    for (fd = loop->anfdmax; fd--; )
        if (loop->anfds [fd].events)
        {
            fd_kill (loop, fd);
            break;
        }
}

/* usually called after fork if backend needs to re-arm all fds from scratch */
static void noinline
fd_rearm_all (struct ev_loop *loop)
{
    int fd;

    for (fd = 0; fd < loop->anfdmax; ++fd)
        if (loop->anfds [fd].events)
        {
            loop->anfds [fd].events = 0;
            loop->anfds [fd].emask  = 0;
            fd_change (loop, fd, EV__IOFDSET | EV_ANFD_REIFY);
        }
}

/* used to prepare libev internal fd's */
/* this is not fork-safe */
inline_speed void
fd_intern (int fd)
{
#ifdef _WIN32
    unsigned long arg = 1;
    ioctlsocket (EV_FD_TO_WIN32_HANDLE (fd), FIONBIO, &arg);
#else
    fcntl (fd, F_SETFD, FD_CLOEXEC);
    fcntl (fd, F_SETFL, O_NONBLOCK);
#endif
}

/*****************************************************************************/

/*
 * the heap functions want a real array index. array index 0 is guaranteed to not
 * be in-use at any time. the first heap entry is at array [HEAP0]. DHEAP gives
 * the branching factor of the d-tree.
 */

/*
 * at the moment we allow libev the luxury of two heaps,
 * a small-code-size 2-heap one and a ~1.5kb larger 4-heap
 * which is more cache-efficient.
 * the difference is about 5% with 50000+ watchers.
 */
#if EV_USE_4HEAP

#define DHEAP 4
#define HEAP0 (DHEAP - 1) /* index of first element in heap */
#define HPARENT(k) ((((k) - HEAP0 - 1) / DHEAP) + HEAP0)
#define UPHEAP_DONE(p,k) ((p) == (k))

/* away from the root */
inline_speed void
downheap (ANHE *heap, int N, int k)
{
    ANHE he = heap [k];
    ANHE *E = heap + N + HEAP0;

    for (;;)
    {
        ev_tstamp minat;
        ANHE *minpos;
        ANHE *pos = heap + DHEAP * (k - HEAP0) + HEAP0 + 1;

        /* find minimum child */
        if (expect_true (pos + DHEAP - 1 < E))
        {
            /* fast path */                               (minpos = pos + 0), (minat = ANHE_at (*minpos));
            if (               ANHE_at (pos [1]) < minat) (minpos = pos + 1), (minat = ANHE_at (*minpos));
            if (               ANHE_at (pos [2]) < minat) (minpos = pos + 2), (minat = ANHE_at (*minpos));
            if (               ANHE_at (pos [3]) < minat) (minpos = pos + 3), (minat = ANHE_at (*minpos));
        }
        else if (pos < E)
        {
            /* slow path */                               (minpos = pos + 0), (minat = ANHE_at (*minpos));
            if (pos + 1 < E && ANHE_at (pos [1]) < minat) (minpos = pos + 1), (minat = ANHE_at (*minpos));
            if (pos + 2 < E && ANHE_at (pos [2]) < minat) (minpos = pos + 2), (minat = ANHE_at (*minpos));
            if (pos + 3 < E && ANHE_at (pos [3]) < minat) (minpos = pos + 3), (minat = ANHE_at (*minpos));
        }
        else
            break;

        if (ANHE_at (he) <= minat)
            break;

        heap [k] = *minpos;
        ev_active (ANHE_w (*minpos)) = k;

        k = minpos - heap;
    }

    heap [k] = he;
    ev_active (ANHE_w (he)) = k;
}

#else /* 4HEAP */

#define HEAP0 1
#define HPARENT(k) ((k) >> 1)
#define UPHEAP_DONE(p,k) (!(p))

/* away from the root */
inline_speed void
downheap (ANHE *heap, int N, int k)
{
    ANHE he = heap [k];

    for (;;)
    {
        int c = k << 1;

        if (c >= N + HEAP0)
            break;

        c += c + 1 < N + HEAP0 && ANHE_at (heap [c]) > ANHE_at (heap [c + 1])
             ? 1 : 0;

        if (ANHE_at (he) <= ANHE_at (heap [c]))
            break;

        heap [k] = heap [c];
        ev_active (ANHE_w (heap [k])) = k;

        k = c;
    }

    heap [k] = he;
    ev_active (ANHE_w (he)) = k;
}
#endif

/* towards the root */
inline_speed void
upheap (ANHE *heap, int k)
{
    ANHE he = heap [k];

    for (;;)
    {
        int p = HPARENT (k);

        if (UPHEAP_DONE (p, k) || ANHE_at (heap [p]) <= ANHE_at (he))
            break;

        heap [k] = heap [p];
        ev_active (ANHE_w (heap [k])) = k;
        k = p;
    }

    heap [k] = he;
    ev_active (ANHE_w (he)) = k;
}

/* move an element suitably so it is in a correct place */
inline_size void
adjustheap (ANHE *heap, int N, int k)
{
    if (k > HEAP0 && ANHE_at (heap [k]) <= ANHE_at (heap [HPARENT (k)]))
        upheap (heap, k);
    else
        downheap (heap, N, k);
}

/* rebuild the heap: this function is used only once and executed rarely */
inline_size void
reheap (ANHE *heap, int N)
{
    int i;

    /* we don't use floyds algorithm, upheap is simpler and is more cache-efficient */
    /* also, this is easy to implement and correct for both 2-heaps and 4-heaps */
    for (i = 0; i < N; ++i)
        upheap (heap, i + HEAP0);
}

/*****************************************************************************/

/* associate signal watchers to a signal signal */
typedef struct
{
    EV_ATOMIC_T pending;
    struct ev_loop *loop;
    WL head;
} ANSIG;

static ANSIG signals [EV_NSIG - 1];

/*****************************************************************************/

#if EV_SIGNAL_ENABLE || EV_ASYNC_ENABLE

static void noinline ecb_cold
evpipe_init (struct ev_loop *loop)
{
    if (!ev_is_active (&loop->pipe_w))
    {
        int fds [2];

# if EV_USE_EVENTFD
        fds [0] = -1;
        fds [1] = eventfd (0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (fds [1] < 0 && errno == EINVAL)
            fds [1] = eventfd (0, 0);

        if (fds [1] < 0)
# endif
        {
            while (pipe (fds))
                ev_syserr ("(libev) error creating signal/async pipe");

            fd_intern (fds [0]);
        }

        fd_intern (fds [1]);

        loop->evpipe [0] = fds [0];

        if (loop->evpipe [1] < 0)
            loop->evpipe [1] = fds [1]; /* first call, set write fd */
        else
        {
            /* on subsequent calls, do not change evpipe [1] */
            /* so that evpipe_write can always rely on its value. */
            /* this branch does not do anything sensible on windows, */
            /* so must not be executed on windows */

            dup2 (fds [1], loop->evpipe [1]);
            close (fds [1]);
        }

        ev_io_set (&loop->pipe_w, loop->evpipe [0] < 0 ? loop->evpipe [1] : loop->evpipe [0], EV_READ);
        ev_io_start (loop, &loop->pipe_w);
        ev_unref (loop); /* watcher should not keep loop alive */
    }
}

inline_speed void
evpipe_write (struct ev_loop *loop, EV_ATOMIC_T *flag)
{
    ECB_MEMORY_FENCE; /* push out the write before this function was called, acquire flag */

    if (expect_true (*flag))
        return;

    *flag = 1;
    ECB_MEMORY_FENCE_RELEASE; /* make sure flag is visible before the wakeup */

    loop->pipe_write_skipped = 1;

    ECB_MEMORY_FENCE; /* make sure pipe_write_skipped is visible before we check pipe_write_wanted */

    if (loop->pipe_write_wanted)
    {
        int old_errno;

        loop->pipe_write_skipped = 0;
        ECB_MEMORY_FENCE_RELEASE;

        old_errno = errno; /* save errno because write will clobber it */

        if (loop->evpipe [0] < 0)
        {
            uint64_t counter = 1;
            write (loop->evpipe [1], &counter, sizeof (uint64_t));
        }
        else
        {
#ifdef _WIN32
            WSABUF buf;
            DWORD sent;
            buf.buf = &buf;
            buf.len = 1;
            WSASend (EV_FD_TO_WIN32_HANDLE (loop->evpipe [1]), &buf, 1, &sent, 0, 0, 0);
#else
            write (loop->evpipe [1], &(loop->evpipe [1]), 1);
#endif
        }

        errno = old_errno;
    }
}

/* called whenever the libev signal pipe */
/* got some events (signal, async) */
static void
pipecb (struct ev_loop *loop, ev_io *iow, int revents)
{
    int i;

    if (revents & EV_READ)
    {
#if EV_USE_EVENTFD
        if (loop->evpipe [0] < 0)
        {
            uint64_t counter;
            read (loop->evpipe [1], &counter, sizeof (uint64_t));
        }
        else
#endif
        {
            char dummy[4];
#ifdef _WIN32
            WSABUF buf;
            DWORD recvd;
            DWORD flags = 0;
            buf.buf = dummy;
            buf.len = sizeof (dummy);
            WSARecv (EV_FD_TO_WIN32_HANDLE (loop->evpipe [0]), &buf, 1, &recvd, &flags, 0, 0);
#else
            read (loop->evpipe [0], &dummy, sizeof (dummy));
#endif
        }
    }

    loop->pipe_write_skipped = 0;

    ECB_MEMORY_FENCE; /* push out skipped, acquire flags */

#if EV_SIGNAL_ENABLE
    if (loop->sig_pending)
    {
        loop->sig_pending = 0;

        ECB_MEMORY_FENCE;

        for (i = EV_NSIG - 1; i--; )
            if (expect_false (signals [i].pending))
                ev_feed_signal_event (loop, i + 1);
    }
#endif

#if EV_ASYNC_ENABLE
    if (async_pending)
    {
        async_pending = 0;

        ECB_MEMORY_FENCE;

        for (i = asynccnt; i--; )
            if (asyncs [i]->sent)
            {
                asyncs [i]->sent = 0;
                ECB_MEMORY_FENCE_RELEASE;
                ev_feed_event (loop, asyncs [i], EV_ASYNC);
            }
    }
#endif
}

/*****************************************************************************/

void
ev_feed_signal (int signum) EV_THROW
{
    struct ev_loop *loop;
    ECB_MEMORY_FENCE_ACQUIRE;
    loop = signals [signum - 1].loop;

    if (!loop)
        return;

    signals [signum - 1].pending = 1;
    evpipe_write (loop, &loop->sig_pending);
}

static void
ev_sighandler (int signum)
{
#ifdef _WIN32
    signal (signum, ev_sighandler);
#endif

    ev_feed_signal (signum);
}

void noinline
ev_feed_signal_event (struct ev_loop *loop, int signum) EV_THROW
{
    WL w;

    if (expect_false (signum <= 0 || signum >= EV_NSIG))
        return;

    --signum;

    /* it is permissible to try to feed a signal to the wrong loop */
    /* or, likely more useful, feeding a signal nobody is waiting for */

    if (expect_false (signals [signum].loop != loop))
        return;

    signals [signum].pending = 0;
    ECB_MEMORY_FENCE_RELEASE;

    for (w = signals [signum].head; w; w = w->next)
        ev_feed_event (loop, (ev_watcher*)w, EV_SIGNAL);
}

#if EV_USE_SIGNALFD
static void
sigfdcb (struct ev_loop *loop, ev_io *iow, int revents)
{
    struct signalfd_siginfo si[2], *sip; /* these structs are big */

    for (;;)
    {
        ssize_t res = read (loop->sigfd, si, sizeof (si));

        /* not ISO-C, as res might be -1, but works with SuS */
        for (sip = si; (char *)sip < (char *)si + res; ++sip)
            ev_feed_signal_event (loop, sip->ssi_signo);

        if (res < (ssize_t)sizeof (si))
            break;
    }
}
#endif

#endif

/*****************************************************************************/

#if EV_CHILD_ENABLE
static WL childs [EV_PID_HASHSIZE];

static ev_signal childev;

#ifndef WIFCONTINUED
# define WIFCONTINUED(status) 0
#endif

/* handle a single child status event */
inline_speed void
child_reap (struct ev_loop *loop, int chain, int pid, int status)
{
    ev_child *w;
    int traced = WIFSTOPPED (status) || WIFCONTINUED (status);

    for (w = (ev_child *)childs [chain & ((EV_PID_HASHSIZE) - 1)]; w; w = (ev_child *)((WL)w)->next)
    {
        if ((w->pid == pid || !w->pid)
                && (!traced || (w->flags & 1)))
        {
            ev_set_priority (w, EV_MAXPRI); /* need to do it *now*, this *must* be the same prio as the signal watcher itself */
            w->rpid    = pid;
            w->rstatus = status;
            ev_feed_event (loop, (ev_watcher*)w, EV_CHILD);
        }
    }
}

#ifndef WCONTINUED
# define WCONTINUED 0
#endif

/* called on sigchld etc., calls waitpid */
static void
childcb (struct ev_loop *loop, ev_signal *sw, int revents)
{
    int pid, status;

    /* some systems define WCONTINUED but then fail to support it (linux 2.4) */
    if (0 >= (pid = waitpid (-1, &status, WNOHANG | WUNTRACED | WCONTINUED)))
        if (!WCONTINUED
                || errno != EINVAL
                || 0 >= (pid = waitpid (-1, &status, WNOHANG | WUNTRACED)))
            return;

    /* make sure we are called again until all children have been reaped */
    /* we need to do it this way so that the callback gets called before we continue */
    ev_feed_event (loop, (ev_watcher*)sw, EV_SIGNAL);

    child_reap (loop, pid, pid, status);
    if ((EV_PID_HASHSIZE) > 1)
        child_reap (loop, 0, pid, status); /* this might trigger a watcher twice, but feed_event catches that */
}

#endif

/*****************************************************************************/

#if EV_USE_IOCP
# include "ev_iocp.c"
#endif
#if EV_USE_PORT
# include "ev_port.c"
#endif
#if EV_USE_KQUEUE
# include "ev_kqueue.c"
#endif
# include "ev_epoll.c"
# include "ev_poll.c"
# include "ev_select.c"

int ecb_cold
ev_version_major (void) EV_THROW
{
    return EV_VERSION_MAJOR;
}

int ecb_cold
ev_version_minor (void) EV_THROW
{
    return EV_VERSION_MINOR;
}

/* return true if we are running with elevated privileges and should ignore env variables */
inline_size ecb_cold int
enable_secure (void)
{
#ifdef _WIN32
    return 0;
#else
    return getuid () != geteuid ()
           || getgid () != getegid ();
#endif
}

unsigned int ecb_cold
ev_supported_backends (void) EV_THROW
{
    unsigned int flags = 0;

    if (EV_USE_PORT  ) flags |= EVBACKEND_PORT;
    if (EV_USE_KQUEUE) flags |= EVBACKEND_KQUEUE;
    if (EV_USE_EPOLL ) flags |= EVBACKEND_EPOLL;
    if (EV_USE_POLL  ) flags |= EVBACKEND_POLL;
    if (EV_USE_SELECT) flags |= EVBACKEND_SELECT;

    return flags;
}

unsigned int ecb_cold
ev_recommended_backends (void) EV_THROW
{
    unsigned int flags = ev_supported_backends ();

#ifndef __NetBSD__
    /* kqueue is borked on everything but netbsd apparently */
    /* it usually doesn't work correctly on anything but sockets and pipes */
    flags &= ~EVBACKEND_KQUEUE;
#endif
#ifdef __APPLE__
    /* only select works correctly on that "unix-certified" platform */
    flags &= ~EVBACKEND_KQUEUE; /* horribly broken, even for sockets */
    flags &= ~EVBACKEND_POLL;   /* poll is based on kqueue from 10.5 onwards */
#endif
#ifdef __FreeBSD__
    flags &= ~EVBACKEND_POLL;   /* poll return value is unusable (http://forums.freebsd.org/archive/index.php/t-10270.html) */
#endif

    return flags;
}

unsigned int ecb_cold
ev_embeddable_backends (void) EV_THROW
{
    int flags = EVBACKEND_EPOLL | EVBACKEND_KQUEUE | EVBACKEND_PORT;

    /* epoll embeddability broken on all linux versions up to at least 2.6.23 */
    if (ev_linux_version () < 0x020620) /* disable it on linux < 2.6.32 */
        flags &= ~EVBACKEND_EPOLL;

    return flags;
}

unsigned int
ev_backend (struct ev_loop *loop) EV_THROW
{
    return loop->backend;
}

#if EV_FEATURE_API
unsigned int
ev_iteration (struct ev_loop *loop) EV_THROW
{
    return loop->loop_count;
}

unsigned int
ev_depth (struct ev_loop *loop) EV_THROW
{
    return loop->loop_depth;
}

void
ev_set_io_collect_interval (struct ev_loop *loop, ev_tstamp interval) EV_THROW
{
    loop->io_blocktime = interval;
}

void
ev_set_timeout_collect_interval (struct ev_loop *loop, ev_tstamp interval) EV_THROW
{
    loop->timeout_blocktime = interval;
}

void
ev_set_userdata (struct ev_loop *loop, void *data) EV_THROW
{
    loop->userdata = data;
}

void *
ev_userdata (struct ev_loop *loop) EV_THROW
{
    return loop->userdata;
}

void
ev_set_invoke_pending_cb (struct ev_loop *loop, void (*invoke_pending_cb)(struct ev_loop *loop)) EV_THROW
{
    loop->invoke_cb = invoke_pending_cb;
}

void
ev_set_loop_release_cb (struct ev_loop *loop, void (*release)(struct ev_loop *loop) EV_THROW, void (*acquire)(struct ev_loop *loop) EV_THROW) EV_THROW
{
    loop->release_cb = release;
    loop->acquire_cb = acquire;
}
#endif

/* initialise a loop structure, must be zero-initialised */
static void noinline ecb_cold
loop_init (struct ev_loop *loop, unsigned int flags) EV_THROW
{
    if (!loop->backend)
    {
        loop->origflags = flags;

#if EV_USE_REALTIME
        if (!have_realtime)
        {
            struct timespec ts;

            if (!clock_gettime (CLOCK_REALTIME, &ts))
                have_realtime = 1;
        }
#endif

#if EV_USE_MONOTONIC
        if (!have_monotonic)
        {
            struct timespec ts;

            if (!clock_gettime (CLOCK_MONOTONIC, &ts))
                have_monotonic = 1;
        }
#endif

        /* pid check not overridable via env */
#ifndef _WIN32
        if (flags & EVFLAG_FORKCHECK)
            loop->curpid = getpid ();
#endif

        if (!(flags & EVFLAG_NOENV)
                && !enable_secure ()
                && getenv ("LIBEV_FLAGS"))
            flags = atoi (getenv ("LIBEV_FLAGS"));

        loop->ev_rt_now          = ev_time ();
        loop->mn_now             = get_clock ();
        loop->now_floor          = loop->mn_now;
        loop->rtmn_diff          = loop->ev_rt_now - loop->mn_now;
#if EV_FEATURE_API
        loop->invoke_cb          = ev_invoke_pending;
#endif

        loop->io_blocktime       = 0.;
        loop->timeout_blocktime  = 0.;
        loop->backend            = 0;
        loop->backend_fd         = -1;
        loop->sig_pending        = 0;
#if EV_ASYNC_ENABLE
        async_pending      = 0;
#endif
        loop->pipe_write_skipped = 0;
        loop->pipe_write_wanted  = 0;
        loop->evpipe [0]         = -1;
        loop->evpipe [1]         = -1;
        loop->fs_fd              = flags & EVFLAG_NOINOTIFY ? -1 : -2;
        loop->sigfd              = flags & EVFLAG_SIGNALFD  ? -2 : -1;

        if (!(flags & EVBACKEND_MASK))
            flags |= ev_recommended_backends ();

#if EV_USE_IOCP
        if (!loop->backend && (flags & EVBACKEND_IOCP  )) loop->backend = iocp_init   (loop, flags);
#endif
#if EV_USE_PORT
        if (!loop->backend && (flags & EVBACKEND_PORT  )) loop->backend = port_init   (loop, flags);
#endif
#if EV_USE_KQUEUE
        if (!loop->backend && (flags & EVBACKEND_KQUEUE)) loop->backend = kqueue_init (loop, flags);
#endif
        if (!loop->backend && (flags & EVBACKEND_EPOLL )) loop->backend = epoll_init  (loop, flags);
        if (!loop->backend && (flags & EVBACKEND_POLL  )) loop->backend = poll_init   (loop, flags);
        if (!loop->backend && (flags & EVBACKEND_SELECT)) loop->backend = select_init (loop, flags);

        ev_prepare_init (&loop->pending_w, pendingcb);

#if EV_SIGNAL_ENABLE || EV_ASYNC_ENABLE
        ev_init (&loop->pipe_w, pipecb);
        ev_set_priority (&loop->pipe_w, EV_MAXPRI);
#endif
    }
}

/* free up a loop structure */
void ecb_cold
ev_loop_destroy (struct ev_loop *loop)
{
    int i;

    /* mimic free (0) */
    if (!loop)
        return;

    /* queue cleanup watchers (and execute them) */
    if (expect_false (loop->cleanupcnt))
    {
        queue_events (loop, (ev_watcher* *)loop->cleanups, loop->cleanupcnt, EV_CLEANUP);
        EV_INVOKE_PENDING;
    }

    if (ev_is_default_loop (loop) && ev_is_active (&childev))
    {
        ev_ref (loop); /* child watcher */
        ev_signal_stop (loop, &childev);
    }

    if (ev_is_active (&loop->pipe_w))
    {
        /*ev_ref (loop);*/
        /*ev_io_stop (loop, &pipe_w);*/

        if (loop->evpipe [0] >= 0) EV_WIN32_CLOSE_FD (loop->evpipe [0]);
        if (loop->evpipe [1] >= 0) EV_WIN32_CLOSE_FD (loop->evpipe [1]);
    }

    if (ev_is_active (&loop->sigfd_w))
        close (loop->sigfd);

    if (loop->fs_fd >= 0)
        close (loop->fs_fd);

    if (loop->backend_fd >= 0)
        close (loop->backend_fd);

#if EV_USE_IOCP
    if (backend == EVBACKEND_IOCP  ) iocp_destroy   (loop);
#endif
#if EV_USE_PORT
    if (backend == EVBACKEND_PORT  ) port_destroy   (loop);
#endif
#if EV_USE_KQUEUE
    if (backend == EVBACKEND_KQUEUE) kqueue_destroy (loop);
#endif
#if EV_USE_EPOLL
    if (loop->backend == EVBACKEND_EPOLL ) epoll_destroy  (loop);
#endif
#if EV_USE_POLL
    if (loop->backend == EVBACKEND_POLL  ) poll_destroy   (loop);
#endif
#if EV_USE_SELECT
    if (loop->backend == EVBACKEND_SELECT) select_destroy (loop);
#endif

    for (i = NUMPRI; i--; )
    {
        array_free (loop->pending, [i]);
#if EV_IDLE_ENABLE
        array_free (idle, [i]);
#endif
    }

    ev_free (loop->anfds);
    loop->anfds = 0;
    loop->anfdmax = 0;

    /* have to use the microsoft-never-gets-it-right macro */
    array_free (loop->rfeed, EMPTY);
    array_free (loop->fdchange, EMPTY);
    array_free (loop->timer, EMPTY);
    array_free (loop->periodic, EMPTY);
    array_free (loop->fork, EMPTY);
    array_free (loop->cleanup, EMPTY);
    array_free (loop->prepare, EMPTY);
    array_free (loop->check, EMPTY);
    array_free (loop->async, EMPTY);

    loop->backend = 0;

    if (ev_is_default_loop (loop))
        ev_default_loop_ptr = 0;
    else
        ev_free (loop);
}

#if EV_USE_INOTIFY
inline_size void infy_fork (struct ev_loop *loop);
#endif

inline_size void
loop_fork (struct ev_loop *loop)
{
#if EV_USE_PORT
    if (backend == EVBACKEND_PORT  ) port_fork   (loop);
#endif
#if EV_USE_KQUEUE
    if (backend == EVBACKEND_KQUEUE) kqueue_fork (loop);
#endif
#if EV_USE_EPOLL
    if (loop->backend == EVBACKEND_EPOLL ) epoll_fork  (loop);
#endif
#if EV_USE_INOTIFY
    infy_fork (loop);
#endif

#if EV_SIGNAL_ENABLE || EV_ASYNC_ENABLE
    if (ev_is_active (&loop->pipe_w))
    {
        /* pipe_write_wanted must be false now, so modifying fd vars should be safe */

        ev_ref (loop);
        ev_io_stop (loop, &loop->pipe_w);

        if (loop->evpipe [0] >= 0)
            EV_WIN32_CLOSE_FD (loop->evpipe [0]);

        evpipe_init (loop);
        /* iterate over everything, in case we missed something before */
        ev_feed_event (loop, &loop->pipe_w, EV_CUSTOM);
    }
#endif

    loop->postfork = 0;
}

struct ev_loop * ecb_cold
ev_loop_new (unsigned int flags) EV_THROW
{
    struct ev_loop *loop = (struct ev_loop *)ev_malloc (sizeof (struct ev_loop));

    memset (loop, 0, sizeof (struct ev_loop));
    loop_init (loop, flags);

    if (ev_backend (loop))
    return loop;

    ev_free (loop);
    return 0;
}

#if EV_VERIFY
static void noinline ecb_cold
verify_watcher (struct ev_loop *loop, ev_watcher* w)
{
    assert (("libev: watcher has invalid priority", ABSPRI (w) >= 0 && ABSPRI (w) < NUMPRI));

    if (w->pending)
        assert (("libev: pending watcher not on pending queue", pendings [ABSPRI (w)][w->pending - 1].w == w));
}

static void noinline ecb_cold
verify_heap (struct ev_loop *loop, ANHE *heap, int N)
{
    int i;

    for (i = HEAP0; i < N + HEAP0; ++i)
    {
        assert (("libev: active index mismatch in heap", ev_active (ANHE_w (heap [i])) == i));
        assert (("libev: heap condition violated", i == HEAP0 || ANHE_at (heap [HPARENT (i)]) <= ANHE_at (heap [i])));
        assert (("libev: heap at cache mismatch", ANHE_at (heap [i]) == ev_at (ANHE_w (heap [i]))));

        verify_watcher (loop, (ev_watcher*)ANHE_w (heap [i]));
    }
}

static void noinline ecb_cold
array_verify (struct ev_loop *loop, ev_watcher* *ws, int cnt)
{
    while (cnt--)
    {
        assert (("libev: active index mismatch", ev_active (ws [cnt]) == cnt + 1));
        verify_watcher (loop, ws [cnt]);
    }
}
#endif

#if EV_FEATURE_API
void ecb_cold
ev_verify (struct ev_loop *loop) EV_THROW
{
#if EV_VERIFY
    int i;
    WL w, w2;

    assert (loop->activecnt >= -1);

    assert (fdchangemax >= fdchangecnt);
    for (i = 0; i < loop->fdchangecnt; ++i)
        assert (("libev: negative fd in fdchanges", fdchanges [i] >= 0));

    assert (loop->anfdmax >= 0);
    for (i = 0; i < loop->anfdmax; ++i)
    {
        int j = 0;

        for (w = w2 = loop->anfds [i].head; w; w = w->next)
        {
            verify_watcher (loop, (ev_watcher*)w);

            if (j++ & 1)
            {
                assert (("libev: io watcher list contains a loop", w != w2));
                w2 = w2->next;
            }

            assert (("libev: inactive fd watcher on anfd list", ev_active (w) == 1));
            assert (("libev: fd mismatch between watcher and anfd", ((ev_io *)w)->fd == i));
        }
    }

    assert (timermax >= timercnt);
    verify_heap (loop, loop->timers, loop->timercnt);

#if EV_PERIODIC_ENABLE
    assert (periodicmax >= periodiccnt);
    verify_heap (loop, periodics, periodiccnt);
#endif

    for (i = NUMPRI; i--; )
    {
        assert (pendingmax [i] >= pendingcnt [i]);
#if EV_IDLE_ENABLE
        assert (idleall >= 0);
        assert (idlemax [i] >= idlecnt [i]);
        array_verify (loop, (ev_watcher* *)idles [i], idlecnt [i]);
#endif
    }

#if EV_FORK_ENABLE
    assert (forkmax >= forkcnt);
    array_verify (loop, (ev_watcher* *)forks, forkcnt);
#endif

#if EV_CLEANUP_ENABLE
    assert (cleanupmax >= cleanupcnt);
    array_verify (loop, (ev_watcher* *)cleanups, cleanupcnt);
#endif

#if EV_ASYNC_ENABLE
    assert (asyncmax >= asynccnt);
    array_verify (loop, (ev_watcher* *)asyncs, asynccnt);
#endif

#if EV_PREPARE_ENABLE
    assert (preparemax >= preparecnt);
    array_verify (loop, (ev_watcher* *)prepares, preparecnt);
#endif

    assert (checkmax >= loop->checkcnt);
    array_verify (loop, (ev_watcher* *)loop->checks, loop->checkcnt);

# if 0
#if EV_CHILD_ENABLE
    for (w = (ev_child *)childs [chain & ((EV_PID_HASHSIZE) - 1)]; w; w = (ev_child *)((WL)w)->next)
        for (signum = EV_NSIG; signum--; ) if (signals [signum].pending)
#endif
# endif
#endif
            }
#endif

struct ev_loop * ecb_cold
ev_default_loop (unsigned int flags) EV_THROW
{
    if (!ev_default_loop_ptr)
{
struct ev_loop *loop = ev_default_loop_ptr = &default_loop_struct;

loop_init (loop, flags);

    if (ev_backend (loop))
    {
#if EV_CHILD_ENABLE
        ev_signal_init (&childev, childcb, SIGCHLD);
        ev_set_priority (&childev, EV_MAXPRI);
        ev_signal_start (loop, &childev);
        ev_unref (loop); /* child watcher should not keep loop alive */
#endif
    }
    else
        ev_default_loop_ptr = 0;
}

return ev_default_loop_ptr;
}

void
ev_loop_fork (struct ev_loop *loop) EV_THROW
{
    loop->postfork = 1;
}

/*****************************************************************************/
inline void EV_CB_INVOKE(ev_watcher* watcher, struct ev_loop* loop, int revents)
{
    watcher->cb(loop, watcher, revents);
}

void
ev_invoke (struct ev_loop *loop, void *w, int revents)
{
    EV_CB_INVOKE ((ev_watcher*)w, loop, revents);
}

unsigned int
ev_pending_count (struct ev_loop *loop) EV_THROW
{
    int pri;
    unsigned int count = 0;

    for (pri = NUMPRI; pri--; )
        count += loop->pendingcnt [pri];

    return count;
}

void noinline
ev_invoke_pending (struct ev_loop *loop)
{
    loop->pendingpri = NUMPRI;

    while (loop->pendingpri) /* pendingpri possibly gets modified in the inner loop */
    {
        --loop->pendingpri;

        while (loop->pendingcnt [loop->pendingpri])
        {
            ANPENDING *p = loop->pendings [loop->pendingpri] + --loop->pendingcnt [loop->pendingpri];

            p->w->pending = 0;
            EV_CB_INVOKE (p->w, loop, p->events);
            EV_FREQUENT_CHECK;
        }
    }
}

/* make idle watchers pending. this handles the "call-idle */
/* only when higher priorities are idle" logic */
inline_size void
idle_reify (struct ev_loop *loop)
{
    if (expect_false (loop->idleall))
    {
        int pri;

        for (pri = NUMPRI; pri--; )
        {
            if (loop->pendingcnt [pri])
                break;

            if (loop->idlecnt [pri])
            {
                queue_events (loop, (ev_watcher* *)loop->idles [pri], loop->idlecnt [pri], EV_IDLE);
                break;
            }
        }
    }
}

/* make timers pending */
inline_size void
timers_reify (struct ev_loop *loop)
{
    EV_FREQUENT_CHECK;

    if (loop->timercnt && ANHE_at (loop->timers [HEAP0]) < loop->mn_now)
    {
        do
        {
            ev_timer *w = (ev_timer *)ANHE_w (loop->timers [HEAP0]);

            /*assert (("libev: inactive timer on timer heap detected", ev_is_active (w)));*/

            /* first reschedule or stop timer */
            if (w->repeat)
            {
                ev_at (w) += w->repeat;
                if (ev_at (w) < loop->mn_now)
                    ev_at (w) = loop->mn_now;

                assert (("libev: negative ev_timer repeat value found while processing timers", w->repeat > 0.));

                ANHE_at_cache (loop->timers [HEAP0]);
                downheap (loop->timers, loop->timercnt, HEAP0);
            }
            else
                ev_timer_stop (loop, w); /* nonrepeating: stop timer */

            EV_FREQUENT_CHECK;
            feed_reverse (loop, (ev_watcher*)w);
        }
        while (loop->timercnt && ANHE_at (loop->timers [HEAP0]) < loop->mn_now);

        feed_reverse_done (loop, EV_TIMER);
    }
}

#if EV_PERIODIC_ENABLE

static void noinline
periodic_recalc (struct ev_loop *loop, ev_periodic *w)
{
    ev_tstamp interval = w->interval > MIN_INTERVAL ? w->interval : MIN_INTERVAL;
    ev_tstamp at = w->offset + interval * ev_floor ((loop->ev_rt_now - w->offset) / interval);

    /* the above almost always errs on the low side */
    while (at <= loop->ev_rt_now)
    {
        ev_tstamp nat = at + w->interval;

        /* when resolution fails us, we use ev_rt_now */
        if (expect_false (nat == at))
        {
            at = loop->ev_rt_now;
            break;
        }

        at = nat;
    }

    ev_at (w) = at;
}

/* make periodics pending */
inline_size void
periodics_reify (struct ev_loop *loop)
{
    EV_FREQUENT_CHECK;

    while (periodiccnt && ANHE_at (periodics [HEAP0]) < loop->ev_rt_now)
    {
        do
        {
            ev_periodic *w = (ev_periodic *)ANHE_w (periodics [HEAP0]);

            /*assert (("libev: inactive timer on periodic heap detected", ev_is_active (w)));*/

            /* first reschedule or stop timer */
            if (w->reschedule_cb)
            {
                ev_at (w) = w->reschedule_cb (w, loop->ev_rt_now);

                assert (("libev: ev_periodic reschedule callback returned time in the past", ev_at (w) >= loop->ev_rt_now));

                ANHE_at_cache (periodics [HEAP0]);
                downheap (periodics, periodiccnt, HEAP0);
            }
            else if (w->interval)
            {
                periodic_recalc (loop, w);
                ANHE_at_cache (periodics [HEAP0]);
                downheap (periodics, periodiccnt, HEAP0);
            }
            else
                ev_periodic_stop (loop, w); /* nonrepeating: stop timer */

            EV_FREQUENT_CHECK;
            feed_reverse (loop, (ev_watcher*)w);
        }
        while (periodiccnt && ANHE_at (periodics [HEAP0]) < loop->ev_rt_now);

        feed_reverse_done (loop, EV_PERIODIC);
    }
}

/* simply recalculate all periodics */
/* TODO: maybe ensure that at least one event happens when jumping forward? */
static void noinline ecb_cold
periodics_reschedule (struct ev_loop *loop)
{
    int i;

    /* adjust periodics after time jump */
    for (i = HEAP0; i < periodiccnt + HEAP0; ++i)
    {
        ev_periodic *w = (ev_periodic *)ANHE_w (periodics [i]);

        if (w->reschedule_cb)
            ev_at (w) = w->reschedule_cb (w, loop->ev_rt_now);
        else if (w->interval)
            periodic_recalc (loop, w);

        ANHE_at_cache (periodics [i]);
    }

    reheap (periodics, periodiccnt);
}
#endif

/* adjust all timers by a given offset */
static void noinline ecb_cold
timers_reschedule (struct ev_loop *loop, ev_tstamp adjust)
{
    int i;

    for (i = 0; i < loop->timercnt; ++i)
    {
        ANHE *he = loop->timers + i + HEAP0;
        ANHE_w (*he)->at += adjust;
        ANHE_at_cache (*he);
    }
}

/* fetch new monotonic and realtime times from the kernel */
/* also detect if there was a timejump, and act accordingly */
inline_speed void
time_update (struct ev_loop *loop, ev_tstamp max_block)
{
#if EV_USE_MONOTONIC
    if (expect_true (have_monotonic))
    {
        int i;
        ev_tstamp odiff = loop->rtmn_diff;

        loop->mn_now = get_clock ();

        /* only fetch the realtime clock every 0.5*MIN_TIMEJUMP seconds */
        /* interpolate in the meantime */
        if (expect_true (loop->mn_now - loop->now_floor < MIN_TIMEJUMP * .5))
        {
            loop->ev_rt_now = loop->rtmn_diff + loop->mn_now;
            return;
        }

        loop->now_floor = loop->mn_now;
        loop->ev_rt_now = ev_time ();

        /* loop a few times, before making important decisions.
         * on the choice of "4": one iteration isn't enough,
         * in case we get preempted during the calls to
         * ev_time and get_clock. a second call is almost guaranteed
         * to succeed in that case, though. and looping a few more times
         * doesn't hurt either as we only do this on time-jumps or
         * in the unlikely event of having been preempted here.
         */
        for (i = 4; --i; )
        {
            ev_tstamp diff;
            loop->rtmn_diff = loop->ev_rt_now - loop->mn_now;

            diff = odiff - loop->rtmn_diff;

            if (expect_true ((diff < 0. ? -diff : diff) < MIN_TIMEJUMP))
                return; /* all is well */

            loop->ev_rt_now = ev_time ();
            loop->mn_now    = get_clock ();
            loop->now_floor = loop->mn_now;
        }

        /* no timer adjustment, as the monotonic clock doesn't jump */
        /* timers_reschedule (loop, rtmn_diff - odiff) */
# if EV_PERIODIC_ENABLE
        periodics_reschedule (loop);
# endif
    }
    else
#endif
    {
        loop->ev_rt_now = ev_time ();

        if (expect_false (loop->mn_now > loop->ev_rt_now || loop->ev_rt_now > loop->mn_now + max_block + MIN_TIMEJUMP))
        {
            /* adjust timers. this is easy, as the offset is the same for all of them */
            timers_reschedule (loop, loop->ev_rt_now - loop->mn_now);
#if EV_PERIODIC_ENABLE
            periodics_reschedule (loop);
#endif
        }

        loop->mn_now = loop->ev_rt_now;
    }
}

int
ev_run (struct ev_loop *loop, int flags)
{
#if EV_FEATURE_API
    ++loop->loop_depth;
#endif

    assert (("libev: ev_loop recursion during release detected", loop_done != EVBREAK_RECURSE));

    loop->loop_done = EVBREAK_CANCEL;

    EV_INVOKE_PENDING; /* in case we recurse, ensure ordering stays nice and clean */

    do
    {
#if EV_VERIFY >= 2
        ev_verify (loop);
#endif

#ifndef _WIN32
        if (expect_false (loop->curpid)) /* penalise the forking check even more */
            if (expect_false (getpid () != loop->curpid))
            {
                loop->curpid = getpid ();
                loop->postfork = 1;
            }
#endif

#if EV_FORK_ENABLE
        /* we might have forked, so queue fork handlers */
        if (expect_false (postfork))
            if (forkcnt)
            {
                queue_events (loop, (ev_watcher* *)forks, forkcnt, EV_FORK);
                EV_INVOKE_PENDING;
            }
#endif

#if EV_PREPARE_ENABLE
        /* queue prepare watchers (and execute them) */
        if (expect_false (preparecnt))
        {
            queue_events (loop, (ev_watcher* *)prepares, preparecnt, EV_PREPARE);
            EV_INVOKE_PENDING;
        }
#endif

        if (expect_false (loop->loop_done))
            break;

        /* we might have forked, so reify kernel state if necessary */
        if (expect_false (loop->postfork))
            loop_fork (loop);

        /* update fd-related kernel structures */
        fd_reify (loop);

        /* calculate blocking time */
        {
            ev_tstamp waittime  = 0.;
            ev_tstamp sleeptime = 0.;

            /* remember old timestamp for io_blocktime calculation */
            ev_tstamp prev_mn_now = loop->mn_now;

            /* update time to cancel out callback processing overhead */
            time_update (loop, 1e100);

            /* from now on, we want a pipe-wake-up */
            loop->pipe_write_wanted = 1;

            ECB_MEMORY_FENCE; /* make sure pipe_write_wanted is visible before we check for potential skips */

            if (expect_true (!(flags & EVRUN_NOWAIT || loop->idleall || !loop->activecnt || loop->pipe_write_skipped)))
            {
                waittime = MAX_BLOCKTIME;

                if (loop->timercnt)
                {
                    ev_tstamp to = ANHE_at (loop->timers [HEAP0]) - loop->mn_now;
                    if (waittime > to) waittime = to;
                }

#if EV_PERIODIC_ENABLE
                if (periodiccnt)
                {
                    ev_tstamp to = ANHE_at (periodics [HEAP0]) - loop->ev_rt_now;
                    if (waittime > to) waittime = to;
                }
#endif

                /* don't let timeouts decrease the waittime below timeout_blocktime */
                if (expect_false (waittime < loop->timeout_blocktime))
                    waittime = loop->timeout_blocktime;

                /* at this point, we NEED to wait, so we have to ensure */
                /* to pass a minimum nonzero value to the backend */
                if (expect_false (waittime < loop->backend_mintime))
                    waittime = loop->backend_mintime;

                /* extra check because io_blocktime is commonly 0 */
                if (expect_false (loop->io_blocktime))
                {
                    sleeptime = loop->io_blocktime - (loop->mn_now - prev_mn_now);

                    if (sleeptime > waittime - loop->backend_mintime)
                        sleeptime = waittime - loop->backend_mintime;

                    if (expect_true (sleeptime > 0.))
                    {
                        ev_sleep (sleeptime);
                        waittime -= sleeptime;
                    }
                }
            }

#if EV_FEATURE_API
            ++loop->loop_count;
#endif
            assert ((loop_done = EVBREAK_RECURSE, 1)); /* assert for side effect */
            loop->backend_poll (loop, waittime);
            assert ((loop_done = EVBREAK_CANCEL, 1)); /* assert for side effect */

            loop->pipe_write_wanted = 0; /* just an optimisation, no fence needed */

            ECB_MEMORY_FENCE_ACQUIRE;
            if (loop->pipe_write_skipped)
            {
                assert (("libev: pipe_w not active, but pipe not written", ev_is_active (&pipe_w)));
                ev_feed_event (loop, &loop->pipe_w, EV_CUSTOM);
            }


            /* update ev_rt_now, do magic */
            time_update (loop, waittime + sleeptime);
        }

        /* queue pending timers and reschedule them */
        timers_reify (loop); /* relative timers called last */
#if EV_PERIODIC_ENABLE
        periodics_reify (loop); /* absolute timers called first */
#endif

#if EV_IDLE_ENABLE
        /* queue idle watchers unless other events are pending */
        idle_reify (loop);
#endif

#if EV_CHECK_ENABLE
        /* queue check watchers, to be executed first */
        if (expect_false (loop->checkcnt))
            queue_events (loop, (ev_watcher* *)checks, loop->checkcnt, EV_CHECK);
#endif

        EV_INVOKE_PENDING;
    }
    while (expect_true (
                loop->activecnt
                && !loop->loop_done
                && !(flags & (EVRUN_ONCE | EVRUN_NOWAIT))
            ));

    if (loop->loop_done == EVBREAK_ONE)
        loop->loop_done = EVBREAK_CANCEL;

#if EV_FEATURE_API
    --loop->loop_depth;
#endif

    return loop->activecnt;
}

void
ev_break (struct ev_loop *loop, int how) EV_THROW
{
    loop->loop_done = how;
}

void
ev_ref (struct ev_loop *loop) EV_THROW
{
    ++loop->activecnt;
}

void
ev_unref (struct ev_loop *loop) EV_THROW
{
    --loop->activecnt;
}

void
ev_now_update (struct ev_loop *loop) EV_THROW
{
    time_update (loop, 1e100);
}

void
ev_suspend (struct ev_loop *loop) EV_THROW
{
    ev_now_update (loop);
}

void
ev_resume (struct ev_loop *loop) EV_THROW
{
    ev_tstamp mn_prev = loop->mn_now;

    ev_now_update (loop);
    timers_reschedule (loop, loop->mn_now - mn_prev);
#if EV_PERIODIC_ENABLE
    /* TODO: really do this? */
    periodics_reschedule (loop);
#endif
}

/*****************************************************************************/
/* singly-linked list management, used when the expected list length is short */

inline_size void
wlist_add (WL *head, WL elem)
{
    elem->next = *head;
    *head = elem;
}

inline_size void
wlist_del (WL *head, WL elem)
{
    while (*head)
    {
        if (expect_true (*head == elem))
        {
            *head = elem->next;
            break;
        }

        head = &(*head)->next;
    }
}

/* internal, faster, version of ev_clear_pending */
inline_speed void
clear_pending (struct ev_loop *loop, ev_watcher* w)
{
    if (w->pending)
    {
        loop->pendings [ABSPRI (w)][w->pending - 1].w = (ev_watcher*)&loop->pending_w;
        w->pending = 0;
    }
}

int
ev_clear_pending (struct ev_loop *loop, void *w) EV_THROW
{
    ev_watcher* w_ = (ev_watcher*)w;
    int pending = w_->pending;

    if (expect_true (pending))
    {
        ANPENDING *p = loop->pendings [ABSPRI (w_)] + pending - 1;
        p->w = (ev_watcher*)&loop->pending_w;
        w_->pending = 0;
        return p->events;
    }
    else
        return 0;
}

inline_size void
pri_adjust (struct ev_loop *loop, ev_watcher* w)
{
    int pri = ev_priority (w);
    pri = pri < EV_MINPRI ? EV_MINPRI : pri;
    pri = pri > EV_MAXPRI ? EV_MAXPRI : pri;
    ev_set_priority (w, pri);
}

inline_speed void
ev_start (struct ev_loop *loop, ev_watcher* w, int active)
{
    pri_adjust (loop, w);
    w->active = active;
    ev_ref (loop);
}

inline_size void
ev_stop (struct ev_loop *loop, ev_watcher* w)
{
    ev_unref (loop);
    w->active = 0;
}

/*****************************************************************************/

void noinline
ev_io_start (struct ev_loop *loop, ev_io *w) EV_THROW
{
    int fd = w->fd;

    if (expect_false (ev_is_active (w)))
        return;

    assert (("libev: ev_io_start called with negative fd", fd >= 0));
    assert (("libev: ev_io_start called with illegal event mask", !(w->events & ~(EV__IOFDSET | EV_READ | EV_WRITE))));

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, 1);
    array_needsize (ANFD, loop->anfds, loop->anfdmax, fd + 1, array_init_zero);
    wlist_add (&loop->anfds[fd].head, (WL)w);

    /* common bug, apparently */
    assert (("libev: ev_io_start called with corrupted watcher", ((WL)w)->next != (WL)w));

    fd_change (loop, fd, w->events & (EV__IOFDSET | EV_ANFD_REIFY));
    w->events &= ~EV__IOFDSET;

    EV_FREQUENT_CHECK;
}

void noinline
ev_io_stop (struct ev_loop *loop, ev_io *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    assert (("libev: ev_io_stop called with illegal fd (must stay constant after start!)", w->fd >= 0 && w->fd < loop->anfdmax));

    EV_FREQUENT_CHECK;

    wlist_del (&loop->anfds[w->fd].head, (WL)w);
    ev_stop (loop, (ev_watcher*)w);

    fd_change (loop, w->fd, EV_ANFD_REIFY);

    EV_FREQUENT_CHECK;
}

void noinline
ev_timer_start (struct ev_loop *loop, ev_timer *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    ev_at (w) += loop->mn_now;

    assert (("libev: ev_timer_start called with negative timer repeat value", w->repeat >= 0.));

    EV_FREQUENT_CHECK;

    ++loop->timercnt;
    ev_start (loop, (ev_watcher*)w, loop->timercnt + HEAP0 - 1);
    array_needsize (ANHE, loop->timers, loop->timermax, ev_active (w) + 1, EMPTY2);
    ANHE_w (loop->timers [ev_active (w)]) = (WT)w;
    ANHE_at_cache (loop->timers [ev_active (w)]);
    upheap (loop->timers, ev_active (w));

    EV_FREQUENT_CHECK;

    /*assert (("libev: internal timer heap corruption", timers [ev_active (w)] == (WT)w));*/
}

void noinline
ev_timer_stop (struct ev_loop *loop, ev_timer *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        assert (("libev: internal timer heap corruption", ANHE_w (timers [active]) == (WT)w));

        --loop->timercnt;

        if (expect_true (active < loop->timercnt + HEAP0))
        {
            loop->timers [active] = loop->timers [loop->timercnt + HEAP0];
            adjustheap (loop->timers, loop->timercnt, active);
        }
    }

    ev_at (w) -= loop->mn_now;

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}

void noinline
ev_timer_again (struct ev_loop *loop, ev_timer *w) EV_THROW
{
    EV_FREQUENT_CHECK;

    clear_pending (loop, (ev_watcher*)w);

    if (ev_is_active (w))
    {
        if (w->repeat)
        {
            ev_at (w) = loop->mn_now + w->repeat;
            ANHE_at_cache (loop->timers [ev_active (w)]);
            adjustheap (loop->timers, loop->timercnt, ev_active (w));
        }
        else
            ev_timer_stop (loop, w);
    }
    else if (w->repeat)
    {
        ev_at (w) = w->repeat;
        ev_timer_start (loop, w);
    }

    EV_FREQUENT_CHECK;
}

ev_tstamp
ev_timer_remaining (struct ev_loop *loop, ev_timer *w) EV_THROW
{
    return ev_at (w) - (ev_is_active (w) ? loop->mn_now : 0.);
}

#if EV_PERIODIC_ENABLE
void noinline
ev_periodic_start (struct ev_loop *loop, ev_periodic *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    if (w->reschedule_cb)
        ev_at (w) = w->reschedule_cb (w, loop->ev_rt_now);
    else if (w->interval)
    {
        assert (("libev: ev_periodic_start called with negative interval value", w->interval >= 0.));
        periodic_recalc (loop, w);
    }
    else
        ev_at (w) = w->offset;

    EV_FREQUENT_CHECK;

    ++periodiccnt;
    ev_start (loop, (ev_watcher*)w, periodiccnt + HEAP0 - 1);
    array_needsize (ANHE, periodics, periodicmax, ev_active (w) + 1, EMPTY2);
    ANHE_w (periodics [ev_active (w)]) = (WT)w;
    ANHE_at_cache (periodics [ev_active (w)]);
    upheap (periodics, ev_active (w));

    EV_FREQUENT_CHECK;

    /*assert (("libev: internal periodic heap corruption", ANHE_w (periodics [ev_active (w)]) == (WT)w));*/
}

void noinline
ev_periodic_stop (struct ev_loop *loop, ev_periodic *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        assert (("libev: internal periodic heap corruption", ANHE_w (periodics [active]) == (WT)w));

        --periodiccnt;

        if (expect_true (active < periodiccnt + HEAP0))
        {
            periodics [active] = periodics [periodiccnt + HEAP0];
            adjustheap (periodics, periodiccnt, active);
        }
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}

void noinline
ev_periodic_again (struct ev_loop *loop, ev_periodic *w) EV_THROW
{
    /* TODO: use adjustheap and recalculation */
    ev_periodic_stop (loop, w);
    ev_periodic_start (loop, w);
}
#endif

#ifndef SA_RESTART
# define SA_RESTART 0
#endif

#if EV_SIGNAL_ENABLE

void noinline
ev_signal_start (struct ev_loop *loop, ev_signal *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    assert (("libev: ev_signal_start called with illegal signal number", w->signum > 0 && w->signum < EV_NSIG));

    assert (("libev: a signal must not be attached to two different loops",
    !signals [w->signum - 1].loop || signals [w->signum - 1].loop == loop));

    signals [w->signum - 1].loop = loop;
    ECB_MEMORY_FENCE_RELEASE;

    EV_FREQUENT_CHECK;

#if EV_USE_SIGNALFD
    if (loop->sigfd == -2)
    {
        loop->sigfd = signalfd (-1, &loop->sigfd_set, SFD_NONBLOCK | SFD_CLOEXEC);
        if (loop->sigfd < 0 && errno == EINVAL)
            loop->sigfd = signalfd (-1, &loop->sigfd_set, 0); /* retry without flags */

        if (loop->sigfd >= 0)
        {
            fd_intern (loop->sigfd); /* doing it twice will not hurt */

            sigemptyset (&loop->sigfd_set);

            ev_io_init (&loop->sigfd_w, sigfdcb, loop->sigfd, EV_READ);
            ev_set_priority (&loop->sigfd_w, EV_MAXPRI);
            ev_io_start (loop, &loop->sigfd_w);
            ev_unref (loop); /* signalfd watcher should not keep loop alive */
        }
    }

    if (loop->sigfd >= 0)
    {
        /* TODO: check .head */
        sigaddset (&loop->sigfd_set, w->signum);
        sigprocmask (SIG_BLOCK, &loop->sigfd_set, 0);

        signalfd (loop->sigfd, &loop->sigfd_set, 0);
    }
#endif

    ev_start (loop, (ev_watcher*)w, 1);
    wlist_add (&signals [w->signum - 1].head, (WL)w);

    if (!((WL)w)->next)
# if EV_USE_SIGNALFD
        if (loop->sigfd < 0) /*TODO*/
# endif
        {
# ifdef _WIN32
            evpipe_init (loop);

            signal (w->signum, ev_sighandler);
# else
            struct sigaction sa;

            evpipe_init (loop);

            sa.sa_handler = ev_sighandler;
            sigfillset (&sa.sa_mask);
            sa.sa_flags = SA_RESTART; /* if restarting works we save one iteration */
            sigaction (w->signum, &sa, 0);

            if (loop->origflags & EVFLAG_NOSIGMASK)
            {
                sigemptyset (&sa.sa_mask);
                sigaddset (&sa.sa_mask, w->signum);
                sigprocmask (SIG_UNBLOCK, &sa.sa_mask, 0);
            }
#endif
        }

    EV_FREQUENT_CHECK;
}

void noinline
ev_signal_stop (struct ev_loop *loop, ev_signal *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    wlist_del (&signals [w->signum - 1].head, (WL)w);
    ev_stop (loop, (ev_watcher*)w);

    if (!signals [w->signum - 1].head)
    {
        signals [w->signum - 1].loop = 0; /* unattach from signal */
#if EV_USE_SIGNALFD
        if (loop->sigfd >= 0)
        {
            sigset_t ss;

            sigemptyset (&ss);
            sigaddset (&ss, w->signum);
            sigdelset (&loop->sigfd_set, w->signum);

            signalfd (loop->sigfd, &loop->sigfd_set, 0);
            sigprocmask (SIG_UNBLOCK, &ss, 0);
        }
        else
#endif
            signal (w->signum, SIG_DFL);
    }

    EV_FREQUENT_CHECK;
}

#endif

#if EV_CHILD_ENABLE

void
ev_child_start (struct ev_loop *loop, ev_child *w) EV_THROW
{
    assert (("libev: child watchers are only supported in the default loop", loop == ev_default_loop_ptr));
    if (expect_false (ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, 1);
    wlist_add (&childs [w->pid & ((EV_PID_HASHSIZE) - 1)], (WL)w);

    EV_FREQUENT_CHECK;
}

void
ev_child_stop (struct ev_loop *loop, ev_child *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    wlist_del (&childs [w->pid & ((EV_PID_HASHSIZE) - 1)], (WL)w);
    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}

#endif

#if EV_STAT_ENABLE

# ifdef _WIN32
#  undef lstat
#  define lstat(a,b) _stati64 (a,b)
# endif

#define DEF_STAT_INTERVAL  5.0074891
#define NFS_STAT_INTERVAL 30.1074891 /* for filesystems potentially failing inotify */
#define MIN_STAT_INTERVAL  0.1074891

static void noinline stat_timer_cb (struct ev_loop *loop, ev_timer *w_, int revents);

#if EV_USE_INOTIFY

/* the * 2 is to allow for alignment padding, which for some reason is >> 8 */
# define EV_INOTIFY_BUFSIZE (sizeof (struct inotify_event) * 2 + NAME_MAX)

static void noinline
infy_add (struct ev_loop *loop, ev_stat *w)
{
    w->wd = inotify_add_watch (fs_fd, w->path,
                               IN_ATTRIB | IN_DELETE_SELF | IN_MOVE_SELF | IN_MODIFY
                               | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO
                               | IN_DONT_FOLLOW | IN_MASK_ADD);

    if (w->wd >= 0)
    {
        struct statfs sfs;

        /* now local changes will be tracked by inotify, but remote changes won't */
        /* unless the filesystem is known to be local, we therefore still poll */
        /* also do poll on <2.6.25, but with normal frequency */

        if (!fs_2625)
            w->timer.repeat = w->interval ? w->interval : DEF_STAT_INTERVAL;
        else if (!statfs (w->path, &sfs)
                 && (sfs.f_type == 0x1373 /* devfs */
                     || sfs.f_type == 0x4006 /* fat */
                     || sfs.f_type == 0x4d44 /* msdos */
                     || sfs.f_type == 0xEF53 /* ext2/3 */
                     || sfs.f_type == 0x72b6 /* jffs2 */
                     || sfs.f_type == 0x858458f6 /* ramfs */
                     || sfs.f_type == 0x5346544e /* ntfs */
                     || sfs.f_type == 0x3153464a /* jfs */
                     || sfs.f_type == 0x9123683e /* btrfs */
                     || sfs.f_type == 0x52654973 /* reiser3 */
                     || sfs.f_type == 0x01021994 /* tmpfs */
                     || sfs.f_type == 0x58465342 /* xfs */))
            w->timer.repeat = 0.; /* filesystem is local, kernel new enough */
        else
            w->timer.repeat = w->interval ? w->interval : NFS_STAT_INTERVAL; /* remote, use reduced frequency */
    }
    else
    {
        /* can't use inotify, continue to stat */
        w->timer.repeat = w->interval ? w->interval : DEF_STAT_INTERVAL;

        /* if path is not there, monitor some parent directory for speedup hints */
        /* note that exceeding the hardcoded path limit is not a correctness issue, */
        /* but an efficiency issue only */
        if ((errno == ENOENT || errno == EACCES) && strlen (w->path) < 4096)
        {
            char path [4096];
            strcpy (path, w->path);

            do
            {
                int mask = IN_MASK_ADD | IN_DELETE_SELF | IN_MOVE_SELF
                           | (errno == EACCES ? IN_ATTRIB : IN_CREATE | IN_MOVED_TO);

                char *pend = strrchr (path, '/');

                if (!pend || pend == path)
                    break;

                *pend = 0;
                w->wd = inotify_add_watch (fs_fd, path, mask);
            }
            while (w->wd < 0 && (errno == ENOENT || errno == EACCES));
        }
    }

    if (w->wd >= 0)
        wlist_add (&fs_hash [w->wd & ((EV_INOTIFY_HASHSIZE) - 1)].head, (WL)w);

    /* now re-arm timer, if required */
    if (ev_is_active (&w->timer)) ev_ref (loop);
    ev_timer_again (loop, &w->timer);
    if (ev_is_active (&w->timer)) ev_unref (loop);
}

static void noinline
infy_del (struct ev_loop *loop, ev_stat *w)
{
    int slot;
    int wd = w->wd;

    if (wd < 0)
        return;

    w->wd = -2;
    slot = wd & ((EV_INOTIFY_HASHSIZE) - 1);
    wlist_del (&fs_hash [slot].head, (WL)w);

    /* remove this watcher, if others are watching it, they will rearm */
    inotify_rm_watch (fs_fd, wd);
}

static void noinline
infy_wd (struct ev_loop *loop, int slot, int wd, struct inotify_event *ev)
{
    if (slot < 0)
        /* overflow, need to check for all hash slots */
        for (slot = 0; slot < (EV_INOTIFY_HASHSIZE); ++slot)
            infy_wd (loop, slot, wd, ev);
    else
    {
        WL w_;

        for (w_ = fs_hash [slot & ((EV_INOTIFY_HASHSIZE) - 1)].head; w_; )
        {
            ev_stat *w = (ev_stat *)w_;
            w_ = w_->next; /* lets us remove this watcher and all before it */

            if (w->wd == wd || wd == -1)
            {
                if (ev->mask & (IN_IGNORED | IN_UNMOUNT | IN_DELETE_SELF))
                {
                    wlist_del (&fs_hash [slot & ((EV_INOTIFY_HASHSIZE) - 1)].head, (WL)w);
                    w->wd = -1;
                    infy_add (loop, w); /* re-add, no matter what */
                }

                stat_timer_cb (loop, &w->timer, 0);
            }
        }
    }
}

static void
infy_cb (struct ev_loop *loop, ev_io *w, int revents)
{
    char buf [EV_INOTIFY_BUFSIZE];
    int ofs;
    int len = read (fs_fd, buf, sizeof (buf));

    for (ofs = 0; ofs < len; )
    {
        struct inotify_event *ev = (struct inotify_event *)(buf + ofs);
        infy_wd (loop, ev->wd, ev->wd, ev);
        ofs += sizeof (struct inotify_event) + ev->len;
    }
}

inline_size void ecb_cold
ev_check_2625 (struct ev_loop *loop)
{
    /* kernels < 2.6.25 are borked
     * http://www.ussg.indiana.edu/hypermail/linux/kernel/0711.3/1208.html
     */
    if (ev_linux_version () < 0x020619)
        return;

    fs_2625 = 1;
}

inline_size int
infy_newfd (void)
{
#if defined IN_CLOEXEC && defined IN_NONBLOCK
    int fd = inotify_init1 (IN_CLOEXEC | IN_NONBLOCK);
    if (fd >= 0)
        return fd;
#endif
    return inotify_init ();
}

inline_size void
infy_init (struct ev_loop *loop)
{
    if (fs_fd != -2)
        return;

    fs_fd = -1;

    ev_check_2625 (loop);

    fs_fd = infy_newfd ();

    if (fs_fd >= 0)
    {
        fd_intern (fs_fd);
        ev_io_init (&fs_w, infy_cb, fs_fd, EV_READ);
        ev_set_priority (&fs_w, EV_MAXPRI);
        ev_io_start (loop, &fs_w);
        ev_unref (loop);
    }
}

inline_size void
infy_fork (struct ev_loop *loop)
{
    int slot;

    if (fs_fd < 0)
        return;

    ev_ref (loop);
    ev_io_stop (loop, &fs_w);
    close (fs_fd);
    fs_fd = infy_newfd ();

    if (fs_fd >= 0)
    {
        fd_intern (fs_fd);
        ev_io_set (&fs_w, fs_fd, EV_READ);
        ev_io_start (loop, &fs_w);
        ev_unref (loop);
    }

    for (slot = 0; slot < (EV_INOTIFY_HASHSIZE); ++slot)
    {
        WL w_ = fs_hash [slot].head;
        fs_hash [slot].head = 0;

        while (w_)
        {
            ev_stat *w = (ev_stat *)w_;
            w_ = w_->next; /* lets us add this watcher */

            w->wd = -1;

            if (fs_fd >= 0)
                infy_add (loop, w); /* re-add, no matter what */
            else
            {
                w->timer.repeat = w->interval ? w->interval : DEF_STAT_INTERVAL;
                if (ev_is_active (&w->timer)) ev_ref (loop);
                ev_timer_again (loop, &w->timer);
                if (ev_is_active (&w->timer)) ev_unref (loop);
            }
        }
    }
}

#endif

#ifdef _WIN32
# define EV_LSTAT(p,b) _stati64 (p, b)
#else
# define EV_LSTAT(p,b) lstat (p, b)
#endif

void
ev_stat_stat (struct ev_loop *loop, ev_stat *w) EV_THROW
{
    if (lstat (w->path, &w->attr) < 0)
        w->attr.st_nlink = 0;
    else if (!w->attr.st_nlink)
        w->attr.st_nlink = 1;
}

static void noinline
stat_timer_cb (struct ev_loop *loop, ev_timer *w_, int revents)
{
    ev_stat *w = (ev_stat *)(((char *)w_) - offsetof (ev_stat, timer));

    ev_statdata prev = w->attr;
    ev_stat_stat (loop, w);

    /* memcmp doesn't work on netbsd, they.... do stuff to their struct stat */
    if (
        prev.st_dev      != w->attr.st_dev
        || prev.st_ino   != w->attr.st_ino
        || prev.st_mode  != w->attr.st_mode
        || prev.st_nlink != w->attr.st_nlink
        || prev.st_uid   != w->attr.st_uid
        || prev.st_gid   != w->attr.st_gid
        || prev.st_rdev  != w->attr.st_rdev
        || prev.st_size  != w->attr.st_size
        || prev.st_atime != w->attr.st_atime
        || prev.st_mtime != w->attr.st_mtime
        || prev.st_ctime != w->attr.st_ctime
    ) {
        /* we only update w->prev on actual differences */
        /* in case we test more often than invoke the callback, */
        /* to ensure that prev is always different to attr */
        w->prev = prev;

#if EV_USE_INOTIFY
        if (fs_fd >= 0)
        {
            infy_del (loop, w);
            infy_add (loop, w);
            ev_stat_stat (loop, w); /* avoid race... */
        }
#endif

        ev_feed_event (loop, w, EV_STAT);
    }
}

void
ev_stat_start (struct ev_loop *loop, ev_stat *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    ev_stat_stat (loop, w);

    if (w->interval < MIN_STAT_INTERVAL && w->interval)
        w->interval = MIN_STAT_INTERVAL;

    ev_timer_init (&w->timer, stat_timer_cb, 0., w->interval ? w->interval : DEF_STAT_INTERVAL);
    ev_set_priority (&w->timer, ev_priority (w));

#if EV_USE_INOTIFY
    infy_init (loop);

    if (fs_fd >= 0)
        infy_add (loop, w);
    else
#endif
    {
        ev_timer_again (loop, &w->timer);
        ev_unref (loop);
    }

    ev_start (loop, (ev_watcher*)w, 1);

    EV_FREQUENT_CHECK;
}

void
ev_stat_stop (struct ev_loop *loop, ev_stat *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

#if EV_USE_INOTIFY
    infy_del (loop, w);
#endif

    if (ev_is_active (&w->timer))
    {
        ev_ref (loop);
        ev_timer_stop (loop, &w->timer);
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_IDLE_ENABLE
void
ev_idle_start (struct ev_loop *loop, ev_idle *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    pri_adjust (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;

    {
        int active = ++idlecnt [ABSPRI (w)];

        ++idleall;
        ev_start (loop, (ev_watcher*)w, active);

        array_needsize (ev_idle *, idles [ABSPRI (w)], idlemax [ABSPRI (w)], active, EMPTY2);
        idles [ABSPRI (w)][active - 1] = w;
    }

    EV_FREQUENT_CHECK;
}

void
ev_idle_stop (struct ev_loop *loop, ev_idle *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        idles [ABSPRI (w)][active - 1] = idles [ABSPRI (w)][--idlecnt [ABSPRI (w)]];
        ev_active (idles [ABSPRI (w)][active - 1]) = active;

        ev_stop (loop, (ev_watcher*)w);
        --idleall;
    }

    EV_FREQUENT_CHECK;
}
#endif

#if EV_PREPARE_ENABLE
void
ev_prepare_start (struct ev_loop *loop, ev_prepare *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, ++preparecnt);
    array_needsize (ev_prepare *, prepares, preparemax, preparecnt, EMPTY2);
    prepares [preparecnt - 1] = w;

    EV_FREQUENT_CHECK;
}

void
ev_prepare_stop (struct ev_loop *loop, ev_prepare *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        prepares [active - 1] = prepares [--preparecnt];
        ev_active (prepares [active - 1]) = active;
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_CHECK_ENABLE
void
ev_check_start (struct ev_loop *loop, ev_check *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, ++loop->checkcnt);
    array_needsize (ev_check *, checks, checkmax, loop->checkcnt, EMPTY2);
    checks [loop->checkcnt - 1] = w;

    EV_FREQUENT_CHECK;
}

void
ev_check_stop (struct ev_loop *loop, ev_check *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        checks [active - 1] = checks [--loop->checkcnt];
        ev_active (checks [active - 1]) = active;
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_EMBED_ENABLE
void noinline
ev_embed_sweep (struct ev_loop *loop, ev_embed *w) EV_THROW
{
    ev_run (w->other, EVRUN_NOWAIT);
}

static void
embed_io_cb (struct ev_loop *loop, ev_io *io, int revents)
{
    ev_embed *w = (ev_embed *)(((char *)io) - offsetof (ev_embed, io));

    if (w->cb)
        ev_feed_event (loop, (ev_watcher*)w, EV_EMBED);
    else
        ev_run (w->other, EVRUN_NOWAIT);
}

static void
embed_prepare_cb (struct ev_loop *loop, ev_prepare *prepare, int revents)
{
    ev_embed *w = (ev_embed *)(((char *)prepare) - offsetof (ev_embed, prepare));

    {
        struct ev_loop *loop = w->other;

        while (fdchangecnt)
        {
            fd_reify (loop);
            ev_run (loop, EVRUN_NOWAIT);
        }
    }
}

static void
embed_fork_cb (struct ev_loop *loop, ev_fork *fork_w, int revents)
{
    ev_embed *w = (ev_embed *)(((char *)fork_w) - offsetof (ev_embed, fork));

    ev_embed_stop (loop, w);

    {
        struct ev_loop *loop = w->other;

        ev_loop_fork (loop);
        ev_run (loop, EVRUN_NOWAIT);
    }

    ev_embed_start (loop, w);
}

#if 0
static void
embed_idle_cb (struct ev_loop *loop, ev_idle *idle, int revents)
{
    ev_idle_stop (loop, idle);
}
#endif

void
ev_embed_start (struct ev_loop *loop, ev_embed *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    {
        struct ev_loop *loop = w->other;
        assert (("libev: loop to be embedded is not embeddable", backend & ev_embeddable_backends ()));
        ev_io_init (&w->io, embed_io_cb, backend_fd, EV_READ);
    }

    EV_FREQUENT_CHECK;

    ev_set_priority (&w->io, ev_priority (w));
    ev_io_start (loop, &w->io);

    ev_prepare_init (&w->prepare, embed_prepare_cb);
    ev_set_priority (&w->prepare, EV_MINPRI);
    ev_prepare_start (loop, &w->prepare);

    ev_fork_init (&w->fork, embed_fork_cb);
    ev_fork_start (loop, &w->fork);

    /*ev_idle_init (&w->idle, e,bed_idle_cb);*/

    ev_start (loop, (ev_watcher*)w, 1);

    EV_FREQUENT_CHECK;
}

void
ev_embed_stop (struct ev_loop *loop, ev_embed *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_io_stop      (loop, &w->io);
    ev_prepare_stop (loop, &w->prepare);
    ev_fork_stop    (loop, &w->fork);

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_FORK_ENABLE
void
ev_fork_start (struct ev_loop *loop, ev_fork *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, ++forkcnt);
    array_needsize (ev_fork *, forks, forkmax, forkcnt, EMPTY2);
    forks [forkcnt - 1] = w;

    EV_FREQUENT_CHECK;
}

void
ev_fork_stop (struct ev_loop *loop, ev_fork *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        forks [active - 1] = forks [--forkcnt];
        ev_active (forks [active - 1]) = active;
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_CLEANUP_ENABLE
void
ev_cleanup_start (struct ev_loop *loop, ev_cleanup *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, ++cleanupcnt);
    array_needsize (ev_cleanup *, cleanups, cleanupmax, cleanupcnt, EMPTY2);
    cleanups [cleanupcnt - 1] = w;

    /* cleanup watchers should never keep a refcount on the loop */
    ev_unref (loop);
    EV_FREQUENT_CHECK;
}

void
ev_cleanup_stop (struct ev_loop *loop, ev_cleanup *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;
    ev_ref (loop);

    {
        int active = ev_active (w);

        cleanups [active - 1] = cleanups [--cleanupcnt];
        ev_active (cleanups [active - 1]) = active;
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}
#endif

#if EV_ASYNC_ENABLE
void
ev_async_start (struct ev_loop *loop, ev_async *w) EV_THROW
{
    if (expect_false (ev_is_active (w)))
        return;

    w->sent = 0;

    evpipe_init (loop);

    EV_FREQUENT_CHECK;

    ev_start (loop, (ev_watcher*)w, ++asynccnt);
    array_needsize (ev_async *, asyncs, asyncmax, asynccnt, EMPTY2);
    asyncs [asynccnt - 1] = w;

    EV_FREQUENT_CHECK;
}

void
ev_async_stop (struct ev_loop *loop, ev_async *w) EV_THROW
{
    clear_pending (loop, (ev_watcher*)w);
    if (expect_false (!ev_is_active (w)))
        return;

    EV_FREQUENT_CHECK;

    {
        int active = ev_active (w);

        asyncs [active - 1] = asyncs [--asynccnt];
        ev_active (asyncs [active - 1]) = active;
    }

    ev_stop (loop, (ev_watcher*)w);

    EV_FREQUENT_CHECK;
}

void
ev_async_send (struct ev_loop *loop, ev_async *w) EV_THROW
{
    w->sent = 1;
    evpipe_write (loop, &async_pending);
}
#endif

/*****************************************************************************/

struct ev_once
{
    ev_io io;
    ev_timer to;
    void (*cb)(int revents, void *arg);
    void *arg;
};

static void
once_cb (struct ev_loop *loop, struct ev_once *once, int revents)
{
    void (*cb)(int revents, void *arg) = once->cb;
    void *arg = once->arg;

    ev_io_stop    (loop, &once->io);
    ev_timer_stop (loop, &once->to);
    ev_free (once);

    cb (revents, arg);
}

static void
once_cb_io (struct ev_loop *loop, ev_io *w, int revents)
{
    struct ev_once *once = (struct ev_once *)(((char *)w) - offsetof (struct ev_once, io));

    once_cb (loop, once, revents | ev_clear_pending (loop, &once->to));
}

static void
once_cb_to (struct ev_loop *loop, ev_timer *w, int revents)
{
    struct ev_once *once = (struct ev_once *)(((char *)w) - offsetof (struct ev_once, to));

    once_cb (loop, once, revents | ev_clear_pending (loop, &once->io));
}

void
ev_once (struct ev_loop *loop, int fd, int events, ev_tstamp timeout, void (*cb)(int revents, void *arg), void *arg) EV_THROW
{
    struct ev_once *once = (struct ev_once *)ev_malloc (sizeof (struct ev_once));

    if (expect_false (!once))
    {
        cb (EV_ERROR | EV_READ | EV_WRITE | EV_TIMER, arg);
        return;
    }

    once->cb  = cb;
    once->arg = arg;

    ev_init (&once->io, once_cb_io);
    if (fd >= 0)
    {
        ev_io_set (&once->io, fd, events);
        ev_io_start (loop, &once->io);
    }

    ev_init (&once->to, once_cb_to);
    if (timeout >= 0.)
    {
        ev_timer_set (&once->to, timeout, 0.);
        ev_timer_start (loop, &once->to);
    }
}

/*****************************************************************************/

#if EV_WALK_ENABLE
void ecb_cold
ev_walk (struct ev_loop *loop, int types, void (*cb)(struct ev_loop *loop, int type, void *w)) EV_THROW
{
    int i, j;
    ev_watcher_list *wl, *wn;

    if (types & (EV_IO | EV_EMBED))
        for (i = 0; i < loop->anfdmax; ++i)
            for (wl = anfds [i].head; wl; )
            {
                wn = wl->next;

#if EV_EMBED_ENABLE
                if (ev_cb ((ev_io *)wl) == embed_io_cb)
                {
                    if (types & EV_EMBED)
                        cb (loop, EV_EMBED, ((char *)wl) - offsetof (struct ev_embed, io));
                }
                else
#endif
#if EV_USE_INOTIFY
                    if (ev_cb ((ev_io *)wl) == infy_cb)
                        ;
                    else
#endif
                        if ((ev_io *)wl != &pipe_w)
                            if (types & EV_IO)
                                cb (loop, EV_IO, wl);

                wl = wn;
            }

    if (types & (EV_TIMER | EV_STAT))
        for (i = timercnt + HEAP0; i-- > HEAP0; )
#if EV_STAT_ENABLE
            /*TODO: timer is not always active*/
            if (ev_cb ((ev_timer *)ANHE_w (timers [i])) == stat_timer_cb)
            {
                if (types & EV_STAT)
                    cb (loop, EV_STAT, ((char *)ANHE_w (timers [i])) - offsetof (struct ev_stat, timer));
            }
            else
#endif
                if (types & EV_TIMER)
                    cb (loop, EV_TIMER, ANHE_w (timers [i]));

#if EV_PERIODIC_ENABLE
    if (types & EV_PERIODIC)
        for (i = periodiccnt + HEAP0; i-- > HEAP0; )
            cb (loop, EV_PERIODIC, ANHE_w (periodics [i]));
#endif

#if EV_IDLE_ENABLE
    if (types & EV_IDLE)
        for (j = NUMPRI; j--; )
            for (i = idlecnt [j]; i--; )
                cb (loop, EV_IDLE, idles [j][i]);
#endif

#if EV_FORK_ENABLE
    if (types & EV_FORK)
        for (i = forkcnt; i--; )
            if (ev_cb (forks [i]) != embed_fork_cb)
                cb (loop, EV_FORK, forks [i]);
#endif

#if EV_ASYNC_ENABLE
    if (types & EV_ASYNC)
        for (i = asynccnt; i--; )
            cb (loop, EV_ASYNC, asyncs [i]);
#endif

#if EV_PREPARE_ENABLE
    if (types & EV_PREPARE)
        for (i = preparecnt; i--; )
# if EV_EMBED_ENABLE
            if (ev_cb (prepares [i]) != embed_prepare_cb)
# endif
                cb (loop, EV_PREPARE, prepares [i]);
#endif

#if EV_CHECK_ENABLE
    if (types & EV_CHECK)
        for (i = loop->checkcnt; i--; )
            cb (loop, EV_CHECK, checks [i]);
#endif

#if EV_SIGNAL_ENABLE
    if (types & EV_SIGNAL)
        for (i = 0; i < EV_NSIG - 1; ++i)
            for (wl = signals [i].head; wl; )
            {
                wn = wl->next;
                cb (loop, EV_SIGNAL, wl);
                wl = wn;
            }
#endif

#if EV_CHILD_ENABLE
    if (types & EV_CHILD)
        for (i = (EV_PID_HASHSIZE); i--; )
            for (wl = childs [i]; wl; )
            {
                wn = wl->next;
                cb (loop, EV_CHILD, wl);
                wl = wn;
            }
#endif
    /* EV_STAT     0x00001000 / * stat data changed */
    /* EV_EMBED    0x00010000 / * embedded event loop needs sweep */
}
#endif

#include "ev_wrap.h"

