/*
 * loop member variable declarations
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
VAR (backend_modify, void (*backend_modify)(struct ev_loop *loop, int fd, int oev, int nev))
VAR (backend_poll  , void (*backend_poll)(struct ev_loop *loop, ev_tstamp timeout))

ANFD *anfds;
int anfdmax;

int evpipe [2];
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

