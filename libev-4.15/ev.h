/*
 * libev native API header
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

#ifndef EV_H_
#define EV_H_

#ifdef __cplusplus
# define EV_CPP(x) x
#else
# define EV_CPP(x)
#endif

#define EV_THROW EV_CPP(throw())

/*****************************************************************************/

#define EV_FEATURES 0x7f

#define EV_FEATURE_CODE     ((EV_FEATURES) &  1)
#define EV_FEATURE_DATA     ((EV_FEATURES) &  2)
#define EV_FEATURE_CONFIG   ((EV_FEATURES) &  4)
#define EV_FEATURE_API      ((EV_FEATURES) &  8)
#define EV_FEATURE_WATCHERS ((EV_FEATURES) & 16)
#define EV_FEATURE_BACKENDS ((EV_FEATURES) & 32)
#define EV_FEATURE_OS       ((EV_FEATURES) & 64)

/* these priorities are inclusive, higher priorities will be invoked earlier */
# define EV_MINPRI (EV_FEATURE_CONFIG ? -2 : 0)
# define EV_MAXPRI (EV_FEATURE_CONFIG ? +2 : 0)

# define EV_MULTIPLICITY EV_FEATURE_CONFIG

#ifndef EV_PERIODIC_ENABLE
# define EV_PERIODIC_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_STAT_ENABLE
# define EV_STAT_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_PREPARE_ENABLE
# define EV_PREPARE_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_CHECK_ENABLE
# define EV_CHECK_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_IDLE_ENABLE
# define EV_IDLE_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_FORK_ENABLE
# define EV_FORK_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_CLEANUP_ENABLE
# define EV_CLEANUP_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_SIGNAL_ENABLE
# define EV_SIGNAL_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_CHILD_ENABLE
# ifdef _WIN32
#  define EV_CHILD_ENABLE 0
# else
#  define EV_CHILD_ENABLE EV_FEATURE_WATCHERS
#endif
#endif

#ifndef EV_ASYNC_ENABLE
# define EV_ASYNC_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_EMBED_ENABLE
# define EV_EMBED_ENABLE EV_FEATURE_WATCHERS
#endif

#ifndef EV_WALK_ENABLE
# define EV_WALK_ENABLE 0 /* not yet */
#endif

    /*****************************************************************************/

#if EV_CHILD_ENABLE && !EV_SIGNAL_ENABLE
# undef EV_SIGNAL_ENABLE
# define EV_SIGNAL_ENABLE 1
#endif

    /*****************************************************************************/


# include <signal.h>

#if EV_STAT_ENABLE
# ifdef _WIN32
#  include <time.h>
#  include <sys/types.h>
# endif
# include <sys/stat.h>
#endif

typedef sig_atomic_t volatile EV_ATOMIC_T;
    typedef double ev_tstamp;

/* support multiple event loops? */
struct ev_loop;

#define EV_VERSION_MAJOR 4
#define EV_VERSION_MINOR 15

/* eventmask, revents, events... */
enum {
    EV_UNDEF    = (int)0xFFFFFFFF, /* guaranteed to be invalid */
    EV_NONE     =            0x00, /* no events */
    EV_READ     =            0x01, /* ev_io detected read will not block */
    EV_WRITE    =            0x02, /* ev_io detected write will not block */
    EV__IOFDSET =            0x80, /* internal use only */
    EV_IO       =         EV_READ, /* alias for type-detection */
    EV_TIMER    =      0x00000100, /* timer timed out */
    EV_PERIODIC =      0x00000200, /* periodic timer timed out */
    EV_SIGNAL   =      0x00000400, /* signal was received */
    EV_CHILD    =      0x00000800, /* child/pid had status change */
    EV_STAT     =      0x00001000, /* stat data changed */
    EV_IDLE     =      0x00002000, /* event loop is idling */
    EV_PREPARE  =      0x00004000, /* event loop about to poll */
    EV_CHECK    =      0x00008000, /* event loop finished poll */
    EV_EMBED    =      0x00010000, /* embedded event loop needs sweep */
    EV_FORK     =      0x00020000, /* event loop resumed in child */
    EV_CLEANUP  =      0x00040000, /* event loop resumed in child */
    EV_ASYNC    =      0x00080000, /* async intra-loop signal */
    EV_CUSTOM   =      0x01000000, /* for use by user code */
    EV_ERROR    = (int)0x80000000  /* sent when an error occurs */
};

typedef void (*ev_cb_type)(struct ev_loop *loop, struct ev_watcher *w, int revents);

    /*
     * struct member types:
     * private: you may look at them, but not change them,
     *          and they might not mean anything to you.
     * ro: can be read anytime, but only changed when the watcher isn't active.
     * rw: can be read and modified anytime, even when the watcher is active.
     *
     * some internal details that might be helpful for debugging:
     *
     * active is either 0, which means the watcher is not active,
     *           or the array index of the watcher (periodics, timers)
     *           or the array index + 1 (most other watchers)
     *           or simply 1 for watchers that aren't in some array.
     * pending is either 0, in which case the watcher isn't,
     *           or the array index + 1 in the pendings array.
     */


/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher
{
    int active; /* private */
    int pending; /* private */
    int priority; /* private */
    void *data; /* can be used to add custom fields to all watchers, while losing binary compatibility */
    ev_cb_type cb; /* private */
} ev_watcher;
/* these may evaluate ev multiple times, and the other arguments at most once */
/* either use ev_init + ev_TYPE_set, or the ev_TYPE_init macro, below, to first initialise a watcher */
#define ev_init(ev,cb_) do {			\
  ((ev_watcher *)(void *)(ev))->active  =	\
  ((ev_watcher *)(void *)(ev))->pending = 0;	\
  ev_set_priority ((ev), 0);			\
  ev_set_cb ((ev), cb_);			\
} while (0)

#define ev_is_pending(ev)                    (0 + ((ev_watcher *)(void *)(ev))->pending) /* ro, true when watcher is waiting for callback invocation */
#define ev_is_active(ev)                     (0 + ((ev_watcher *)(void *)(ev))->active) /* ro, true when the watcher has been started */
#define ev_set_cb(ev,cb_)                    (ev)->cb = (ev_cb_type)(cb_)
# define ev_priority(ev)                     (+(((ev_watcher *)(void *)(ev))->priority))
# define ev_set_priority(ev,pri)             (   (ev_watcher *)(void *)(ev))->priority = (pri)


/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_list : ev_watcher
{
    struct ev_watcher_list *next; /* private */
} ev_watcher_list;

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_time : ev_watcher
{
    ev_tstamp at;     /* private */
} ev_watcher_time;

/* invoked when fd is either EV_READable or EV_WRITEable */
/* revent EV_READ, EV_WRITE */
typedef struct ev_io : ev_watcher_list
{
    int fd;     /* ro */
    int events; /* ro */
} ev_io;
#define ev_io_init(ev,cb,fd,events)  do { ev_init ((ev), (cb)); ev_io_set ((ev),(fd),(events)); } while (0)
#define ev_io_set(ev,fd_,events_)    do { (ev)->fd = (fd_); (ev)->events = (events_) | EV__IOFDSET; } while (0)
extern void ev_io_start       (struct ev_loop *loop, ev_io *w) EV_THROW;
extern void ev_io_stop        (struct ev_loop *loop, ev_io *w) EV_THROW;

/* invoked after a specific time, repeatable (based on monotonic clock) */
/* revent EV_TIMEOUT */
typedef struct ev_timer : ev_watcher
{
    ev_tstamp at;     /* private */
    ev_tstamp repeat; /* rw */
} ev_timer;
#define ev_timer_init(ev,cb,after,repeat)    do { ev_init ((ev), (cb)); ev_timer_set ((ev),(after),(repeat)); } while (0)
#define ev_timer_set(ev,after_,repeat_)      do { ((ev_watcher_time *)(ev))->at = (after_); (ev)->repeat = (repeat_); } while (0)
extern void ev_timer_start    (struct ev_loop *loop, ev_timer *w) EV_THROW;
extern void ev_timer_stop     (struct ev_loop *loop, ev_timer *w) EV_THROW;
/* stops if active and no repeat, restarts if active and repeating, starts if inactive and repeating */
extern void ev_timer_again    (struct ev_loop *loop, ev_timer *w) EV_THROW;
/* return remaining time */
extern ev_tstamp ev_timer_remaining (struct ev_loop *loop, ev_timer *w) EV_THROW;


/* invoked at some specific time, possibly repeating at regular intervals (based on UTC) */
/* revent EV_PERIODIC */
typedef struct ev_periodic : ev_watcher
{
    ev_tstamp at;     /* private */
    ev_tstamp offset; /* rw */
    ev_tstamp interval; /* rw */
    ev_tstamp (*reschedule_cb)(struct ev_periodic *w, ev_tstamp now) EV_THROW; /* rw */
} ev_periodic;
#define ev_periodic_init(ev,cb,ofs,ival,rcb) do { ev_init ((ev), (cb)); ev_periodic_set ((ev),(ofs),(ival),(rcb)); } while (0)
#define ev_periodic_set(ev,ofs_,ival_,rcb_)  do { (ev)->offset = (ofs_); (ev)->interval = (ival_); (ev)->reschedule_cb = (rcb_); } while (0)
extern void ev_periodic_start (struct ev_loop *loop, ev_periodic *w) EV_THROW;
extern void ev_periodic_stop  (struct ev_loop *loop, ev_periodic *w) EV_THROW;
extern void ev_periodic_again (struct ev_loop *loop, ev_periodic *w) EV_THROW;

/* invoked when the given signal has been received */
/* revent EV_SIGNAL */
typedef struct ev_signal : ev_watcher_list
{
    int signum; /* ro */
#define ev_signal_init(ev,cb,signum)  do { ev_init ((ev), (cb)); ev_signal_set ((ev), (signum)); } while (0)
#define ev_signal_set(ev,signum_)            do { (ev)->signum = (signum_); } while (0)
} ev_signal;
/* only supported in the default loop */
extern void ev_signal_start   (struct ev_loop *loop, ev_signal *w) EV_THROW;
extern void ev_signal_stop    (struct ev_loop *loop, ev_signal *w) EV_THROW;


/* invoked when sigchld is received and waitpid indicates the given pid */
/* revent EV_CHILD */
/* does not support priorities */
typedef struct ev_child : ev_watcher_list
{
    int flags;   /* private */
    int pid;     /* ro */
    int rpid;    /* rw, holds the received pid */
    int rstatus; /* rw, holds the exit status, use the macros from sys/wait.h */
} ev_child;
#define ev_child_init(ev,cb,pid,trace)       do { ev_init ((ev), (cb)); ev_child_set ((ev),(pid),(trace)); } while (0)
#define ev_child_set(ev,pid_,trace_)         do { (ev)->pid = (pid_); (ev)->flags = !!(trace_); } while (0)
/* only supported in the default loop */
extern void ev_child_start    (struct ev_loop *loop, ev_child *w) EV_THROW;
extern void ev_child_stop     (struct ev_loop *loop, ev_child *w) EV_THROW;


/* st_nlink = 0 means missing file or other error */
# ifdef _WIN32
typedef struct _stati64 ev_statdata;
# else
typedef struct stat ev_statdata;
# endif

/* invoked each time the stat data changes for a given path */
/* revent EV_STAT */
typedef struct ev_stat : ev_watcher_list
{
    ev_timer timer;     /* private */
    ev_tstamp interval; /* ro */
    const char *path;   /* ro */
    ev_statdata prev;   /* ro */
    ev_statdata attr;   /* ro */

    int wd; /* wd for inotify, fd for kqueue */
} ev_stat;
#define ev_stat_init(ev,cb,path,interval)    do { ev_init ((ev), (cb)); ev_stat_set ((ev),(path),(interval)); } while (0)
#define ev_stat_set(ev,path_,interval_)      do { (ev)->path = (path_); (ev)->interval = (interval_); (ev)->wd = -2; } while (0)
extern void ev_stat_start     (struct ev_loop *loop, ev_stat *w) EV_THROW;
extern void ev_stat_stop      (struct ev_loop *loop, ev_stat *w) EV_THROW;
extern void ev_stat_stat      (struct ev_loop *loop, ev_stat *w) EV_THROW;

/* invoked when the nothing else needs to be done, keeps the process from blocking */
/* revent EV_IDLE */
typedef struct ev_idle : ev_watcher
{
} ev_idle;
#define ev_idle_set(ev)                      /* nop, yes, this is a serious in-joke */
#define ev_idle_init(ev,cb)                  do { ev_init ((ev), (cb)); ev_idle_set ((ev)); } while (0)
extern void ev_idle_start     (struct ev_loop *loop, ev_idle *w) EV_THROW;
extern void ev_idle_stop      (struct ev_loop *loop, ev_idle *w) EV_THROW;

/* invoked for each run of the mainloop, just before the blocking call */
/* you can still change events in any way you like */
/* revent EV_PREPARE */
typedef struct ev_prepare : ev_watcher
{
} ev_prepare;
#define ev_prepare_init(ev,cb)               do { ev_init ((ev), (cb)); ev_prepare_set ((ev)); } while (0)
#define ev_prepare_set(ev)                   /* nop, yes, this is a serious in-joke */
extern void ev_prepare_start  (struct ev_loop *loop, ev_prepare *w) EV_THROW;
extern void ev_prepare_stop   (struct ev_loop *loop, ev_prepare *w) EV_THROW;

/* invoked for each run of the mainloop, just after the blocking call */
/* revent EV_CHECK */
typedef struct ev_check : ev_watcher
{
} ev_check;
#define ev_check_set(ev)                     /* nop, yes, this is a serious in-joke */
#define ev_check_init(ev,cb)                 do { ev_init ((ev), (cb)); ev_check_set ((ev)); } while (0)
extern void ev_check_start    (struct ev_loop *loop, ev_check *w) EV_THROW;
extern void ev_check_stop     (struct ev_loop *loop, ev_check *w) EV_THROW;


/* the callback gets invoked before check in the child process when a fork was detected */
/* revent EV_FORK */
typedef struct ev_fork : ev_watcher
{
} ev_fork;
#define ev_fork_set(ev)                      /* nop, yes, this is a serious in-joke */
#define ev_fork_init(ev,cb)                  do { ev_init ((ev), (cb)); ev_fork_set ((ev)); } while (0)
extern void ev_fork_start     (struct ev_loop *loop, ev_fork *w) EV_THROW;
extern void ev_fork_stop      (struct ev_loop *loop, ev_fork *w) EV_THROW;

/* is invoked just before the loop gets destroyed */
/* revent EV_CLEANUP */
typedef struct ev_cleanup : ev_watcher
{
} ev_cleanup;
#define ev_cleanup_init(ev,cb)               do { ev_init ((ev), (cb)); ev_cleanup_set ((ev)); } while (0)
#define ev_cleanup_set(ev)                   /* nop, yes, this is a serious in-joke */
extern void ev_cleanup_start  (struct ev_loop *loop, ev_cleanup *w) EV_THROW;
extern void ev_cleanup_stop   (struct ev_loop *loop, ev_cleanup *w) EV_THROW;

/* used to embed an event loop inside another */
/* the callback gets invoked when the event loop has handled events, and can be 0 */
typedef struct ev_embed : ev_watcher
{
    struct ev_loop *other; /* ro */
    ev_io io;              /* private */
    ev_prepare prepare;    /* private */
    ev_check check;        /* unused */
    ev_timer timer;        /* unused */
    ev_periodic periodic;  /* unused */
    ev_idle idle;          /* unused */
    ev_fork fork;          /* private */
    ev_cleanup cleanup;    /* unused */
} ev_embed;
#define ev_embed_set(ev,other_)              do { (ev)->other = (other_); } while (0)
#define ev_embed_init(ev,cb,other)           do { ev_init ((ev), (cb)); ev_embed_set ((ev),(other)); } while (0)

/* only supported when loop to be embedded is in fact embeddable */
extern void ev_embed_start    (struct ev_loop *loop, ev_embed *w) EV_THROW;
extern void ev_embed_stop     (struct ev_loop *loop, ev_embed *w) EV_THROW;
extern void ev_embed_sweep    (struct ev_loop *loop, ev_embed *w) EV_THROW;


/* invoked when somebody calls ev_async_send on the watcher */
/* revent EV_ASYNC */
typedef struct ev_async : ev_watcher
{
    EV_ATOMIC_T sent; /* private */
} ev_async;
#define ev_async_init(ev,cb)                 do { ev_init ((ev), (cb)); ev_async_set ((ev)); } while (0)
#define ev_async_set(ev)                     /* nop, yes, this is a serious in-joke */
# define ev_async_pending(w) (+(w)->sent)
extern void ev_async_start    (struct ev_loop *loop, ev_async *w) EV_THROW;
extern void ev_async_stop     (struct ev_loop *loop, ev_async *w) EV_THROW;
extern void ev_async_send     (struct ev_loop *loop, ev_async *w) EV_THROW;

/* flag bits for ev_default_loop and ev_loop_new */
enum {
    /* the default */
    EVFLAG_AUTO      = 0x00000000U, /* not quite a mask */
    /* flag bits */
    EVFLAG_NOENV     = 0x01000000U, /* do NOT consult environment */
    EVFLAG_FORKCHECK = 0x02000000U, /* check for a fork in each iteration */
    /* debugging/feature disable */
    EVFLAG_NOINOTIFY = 0x00100000U, /* do not attempt to use inotify */
    EVFLAG_SIGNALFD  = 0x00200000U, /* attempt to use signalfd */
    EVFLAG_NOSIGMASK = 0x00400000U  /* avoid modifying the signal mask */
};

/* method bits to be ored together */
enum {
    EVBACKEND_SELECT  = 0x00000001U, /* about anywhere */
    EVBACKEND_POLL    = 0x00000002U, /* !win */
    EVBACKEND_EPOLL   = 0x00000004U, /* linux */
    EVBACKEND_KQUEUE  = 0x00000008U, /* bsd */
    EVBACKEND_DEVPOLL = 0x00000010U, /* solaris 8 */ /* NYI */
    EVBACKEND_PORT    = 0x00000020U, /* solaris 10 */
    EVBACKEND_ALL     = 0x0000003FU, /* all known backends */
    EVBACKEND_MASK    = 0x0000FFFFU  /* all future backends */
};

extern int ev_version_major (void) EV_THROW;
extern int ev_version_minor (void) EV_THROW;

extern unsigned int ev_supported_backends (void) EV_THROW;
extern unsigned int ev_recommended_backends (void) EV_THROW;
extern unsigned int ev_embeddable_backends (void) EV_THROW;

extern ev_tstamp ev_time (void) EV_THROW;
extern void ev_sleep (ev_tstamp delay) EV_THROW; /* sleep for a while */

/* Sets the allocation function to use, works like realloc.
 * It is used to allocate and free memory.
 * If it returns zero when memory needs to be allocated, the library might abort
 * or take some potentially destructive action.
 * The default is your system realloc function.
 */
extern void ev_set_allocator (void *(*cb)(void *ptr, long size) EV_THROW) EV_THROW;

/* set the callback function to call on a
 * retryable syscall error
 * (such as failed select, poll, epoll_wait)
 */
extern void ev_set_syserr_cb (void (*cb)(const char *msg) EV_THROW) EV_THROW;

/* the default loop is the only one that handles signals and child watchers */
/* you can call this as often as you like */
extern struct ev_loop *ev_default_loop (unsigned int flags = 0) EV_THROW;

static inline struct ev_loop *
ev_default_loop_uc_ (void) EV_THROW
{
    extern struct ev_loop *ev_default_loop_ptr;
    return ev_default_loop_ptr;
}

static inline int
ev_is_default_loop (struct ev_loop *loop) EV_THROW
{
    return loop == ev_default_loop_uc_();
}

/* create and destroy alternative loops that don't handle signals */
extern struct ev_loop *ev_loop_new (unsigned int flags = 0) EV_THROW;

extern ev_tstamp ev_now (struct ev_loop *loop) EV_THROW; /* time w.r.t. timers and the eventloop, updated after each poll */

/* destroy event loops, also works for the default loop */
extern void ev_loop_destroy (struct ev_loop *loop);

/* this needs to be called after fork, to duplicate the loop */
/* when you want to re-use it in the child */
/* you can call it in either the parent or the child */
/* you can actually call it at any time, anywhere :) */
extern void ev_loop_fork (struct ev_loop *loop) EV_THROW;

extern unsigned int ev_backend (struct ev_loop *loop) EV_THROW; /* backend in use by loop */

extern void ev_now_update (struct ev_loop *loop) EV_THROW; /* update event loop time */

/* walk (almost) all watchers in the loop of a given type, invoking the */
/* callback on every such watcher. The callback might stop the watcher, */
/* but do nothing else with the loop */
extern void ev_walk (struct ev_loop *loop, int types, void (*cb)(struct ev_loop *loop, int type, void *w)) EV_THROW;

/* ev_run flags values */
enum {
    EVRUN_NOWAIT = 1, /* do not block/wait */
    EVRUN_ONCE   = 2  /* block *once* only */
};

/* ev_break how values */
enum {
    EVBREAK_CANCEL = 0, /* undo unloop */
    EVBREAK_ONE    = 1, /* unloop once */
    EVBREAK_ALL    = 2  /* unloop all loops */
};

extern int  ev_run (struct ev_loop *loop, int flags = 0);
extern void ev_break (struct ev_loop *loop, int how = EVBREAK_ONE) EV_THROW; /* break out of the loop */

/*
 * ref/unref can be used to add or remove a refcount on the mainloop. every watcher
 * keeps one reference. if you have a long-running watcher you never unregister that
 * should not keep ev_loop from running, unref() after starting, and ref() before stopping.
 */
extern void ev_ref   (struct ev_loop *loop) EV_THROW;
extern void ev_unref (struct ev_loop *loop) EV_THROW;

/*
 * convenience function, wait for a single event, without registering an event watcher
 * if timeout is < 0, do wait indefinitely
 */
extern void ev_once (struct ev_loop *loop, int fd, int events, ev_tstamp timeout, void (*cb)(int revents, void *arg), void *arg) EV_THROW;

extern unsigned int ev_iteration (struct ev_loop *loop) EV_THROW; /* number of loop iterations */
extern unsigned int ev_depth     (struct ev_loop *loop) EV_THROW; /* #ev_loop enters - #ev_loop leaves */
extern void         ev_verify    (struct ev_loop *loop) EV_THROW; /* abort if loop data corrupted */

extern void ev_set_io_collect_interval (struct ev_loop *loop, ev_tstamp interval) EV_THROW; /* sleep at least this time, default 0 */
extern void ev_set_timeout_collect_interval (struct ev_loop *loop, ev_tstamp interval) EV_THROW; /* sleep at least this time, default 0 */

/* advanced stuff for threading etc. support, see docs */
extern void ev_set_userdata (struct ev_loop *loop, void *data) EV_THROW;
extern void *ev_userdata (struct ev_loop *loop) EV_THROW;
extern void ev_set_invoke_pending_cb (struct ev_loop *loop, void (*invoke_pending_cb)(struct ev_loop *loop)) EV_THROW;
extern void ev_set_loop_release_cb (struct ev_loop *loop, void (*release)(struct ev_loop *loop), void (*acquire)(struct ev_loop *loop) EV_THROW) EV_THROW;

extern unsigned int ev_pending_count (struct ev_loop *loop) EV_THROW; /* number of pending events, if any */
extern void ev_invoke_pending (struct ev_loop *loop); /* invoke all pending watchers */

/*
 * stop/start the timer handling.
 */
extern void ev_suspend (struct ev_loop *loop) EV_THROW;
extern void ev_resume  (struct ev_loop *loop) EV_THROW;



#define ev_periodic_at(ev)                   (+((ev_watcher_time *)(ev))->at)

/* stopping (enabling, adding) a watcher does nothing if it is already running */
/* stopping (disabling, deleting) a watcher does nothing unless its already running */
/* feeds an event into a watcher as if the event actually occurred */
/* accepts any ev_watcher type */
extern void ev_feed_event     (struct ev_loop *loop, void *w, int revents) EV_THROW;
extern void ev_feed_fd_event  (struct ev_loop *loop, int fd, int revents) EV_THROW;
extern void ev_feed_signal    (int signum) EV_THROW;
extern void ev_feed_signal_event (struct ev_loop *loop, int signum) EV_THROW;
extern void ev_invoke         (struct ev_loop *loop, void *w, int revents);
extern int  ev_clear_pending  (struct ev_loop *loop, void *w) EV_THROW;

typedef struct ev_loop ev_loop;

#endif

