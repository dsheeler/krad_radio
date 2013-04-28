#include "krad_ticker.h"

static inline uint64_t ts_to_nsec (struct timespec ts) {
  return (ts.tv_sec * 1000000000LL) + ts.tv_nsec;
}

static inline struct timespec nsec_to_ts (uint64_t nsecs) {
  struct timespec ts;
  ts.tv_sec = nsecs / (1000000000LL);
  ts.tv_nsec = nsecs % (1000000000LL);
  return ts;
}

struct timespec timespec_add_ns (struct timespec ts, uint64_t ns) {
  uint64_t nsecs = ts_to_nsec(ts);
  nsecs += ns;
  return nsec_to_ts (nsecs);
}

struct timespec timespec_add_ms (struct timespec ts, uint64_t ms) {
  return timespec_add_ns (ts, ms * 1000000);
}

void krad_ticker_destroy (krad_ticker_t *ticker) {
  free (ticker);
}

krad_ticker_t *krad_ticker_create (int numerator, int denominator) {

  krad_ticker_t *ticker;
  
  ticker = calloc (1, sizeof (krad_ticker_t));

  ticker->period_time_ns = (1000000000 / numerator) * denominator;

  return ticker;
}

krad_ticker_t *krad_ticker_throttle_create () {

  krad_ticker_t *ticker;
  
  ticker = calloc (1, sizeof (krad_ticker_t));
  return ticker;
}

void krad_ticker_start (krad_ticker_t *ticker) {
  ticker->total_periods = 0;
  clock_gettime (CLOCK_MONOTONIC, &ticker->start_time);
}

void krad_ticker_start_at (krad_ticker_t *ticker, struct timespec start_time) {
  ticker->total_periods = 0;
  memcpy (&ticker->start_time, &start_time, sizeof(struct timespec));
  krad_ticker_wait (ticker);
}

void krad_ticker_throttle (krad_ticker_t *ticker, uint64_t ms) {

  ticker->total_ms += ms;

  ticker->wakeup_time = timespec_add_ms (ticker->start_time, ticker->total_ms);

  if (clock_nanosleep (CLOCK_MONOTONIC,
                       TIMER_ABSTIME,
                       &ticker->wakeup_time,
                       NULL)) {
    failfast ("Krad Ticker: error while clock nanosleeping");
  }
}

void krad_ticker_wait (krad_ticker_t *ticker) {

  ticker->wakeup_time = timespec_add_ns (ticker->start_time,
                                         ticker->period_time_ns *
                                         ticker->total_periods);

  if (clock_nanosleep (CLOCK_MONOTONIC,
                       TIMER_ABSTIME,
                       &ticker->wakeup_time,
                       NULL)) {
    failfast ("Krad Ticker: error while clock nanosleeping");
  }

  ticker->total_periods++;
}
