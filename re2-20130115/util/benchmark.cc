// Copyright 2009 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "thirdparty/re2-20130115/util/util.h"
#include "thirdparty/re2-20130115/util/flags.h"
#include "thirdparty/re2-20130115/util/benchmark.h"

DEFINE_string(test_tmpdir, "/var/tmp", "temp directory");

int nbenchmarks;
testing::Benchmark* benchmarks[10000];

namespace testing {
const char kColorCyan[] = "\033[;36m";
const char kColorPurple[] = "\033[;35m";
const char kColorBlue[] = "\033[;34m";
const char kColorYellow[] = "\033[;33m";
const char kColorGreen[] = "\033[;32m";
const char kColorEnd[] = "\033[0m";

void Benchmark::Register() {
    benchmarks[nbenchmarks] = this;
    if(lo < 1)
        lo = 1;
    if(hi < lo)
        hi = lo;
    nbenchmarks++;
}

static int64_t nsec() {
    struct timeval tv;
    if(gettimeofday(&tv, 0) < 0)
        return -1;
    return (int64_t)tv.tv_sec*1000*1000*1000 + tv.tv_usec*1000;
}

static int64_t bytes;
static int64_t ns;
static int64_t t0;
static int64_t items;

void SetBenchmarkBytesProcessed(long long x) {
    bytes = x;
}

void StopBenchmarkTiming() {
    if(t0 != 0)
        ns += nsec() - t0;
    t0 = 0;
}

void StartBenchmarkTiming() {
    if(t0 == 0)
        t0 = nsec();
}

void SetBenchmarkItemsProcessed(int n) {
    items = n;
}

void BenchmarkMemoryUsage() {
    // TODO(rsc): Implement.
}

static void runN(Benchmark *b, int n, int siz) {
    bytes = 0;
    items = 0;
    ns = 0;
    t0 = nsec();
    if(b->fn)
        b->fn(n);
    else if(b->fnr)
        b->fnr(n, siz);
    else {
        fprintf(stderr, "%s: missing function\n", b->name);
        exit(2);
    }
    if(t0 != 0)
        ns += nsec() - t0;
}

static int round(int n) {
    int base = 1;

    while(base*10 < n)
        base *= 10;
    if(n < 2*base)
        return 2*base;
    if(n < 5*base)
        return 5*base;
    return 10*base;
}

void RunBench(Benchmark* b, int nthread, int siz) {
    int n, last;

    // TODO(rsc): Threaded benchmarks.
    if(nthread != 1)
        return;

    // run once in case it's expensive
    n = 1;
    runN(b, n, siz);
    while(ns < (int)1e9 && n < (int)1e9) {
        last = n;
        if(ns/n == 0)
            n = 1e9;
        else
            n = 1e9 / (ns/n);

        n = max(last+1, min(n+n/2, 100*last));
        n = round(n);
        runN(b, n, siz);
    }

    char mb[100];
    char suf[100];
    mb[0] = '\0';
    suf[0] = '\0';
    if(ns > 0 && bytes > 0)
        snprintf(mb, sizeof mb, "\t%7.2f MB/s", ((double)bytes/1e6)/((double)ns/1e9));
    if(b->fnr || b->lo != b->hi) {
        if(siz >= (1<<20))
            snprintf(suf, sizeof suf, "/%dM", siz/(1<<20));
        else if(siz >= (1<<10))
            snprintf(suf, sizeof suf, "/%dK", siz/(1<<10));
        else
            snprintf(suf, sizeof suf, "/%d", siz);
    }
    printf("%s%s%s%s\t%s%8lld\t%s%10lld ns/op%s%s%s\n",
           kColorCyan, b->name,
           kColorPurple, suf,
           kColorBlue, (long long) n,
           kColorYellow, (long long) ns / n,
           kColorGreen, mb,
           kColorEnd);
    fflush(stdout);
}
}
