#include <time.h>

#include "krad_system.h"

typedef struct krad_ticker_St krad_ticker_t;

struct krad_ticker_St {

  struct timespec start_time;
  struct timespec wakeup_time;

  uint64_t period_time_ns;
  uint64_t total_periods;

  uint64_t total_ms;
};

struct timespec timespec_add_ms (struct timespec ts, uint64_t ms);
struct timespec timespec_add_ns (struct timespec ts, uint64_t ns);

void krad_ticker_destroy (krad_ticker_t *ticker);
krad_ticker_t *krad_ticker_create (int numerator, int denominator);
krad_ticker_t *krad_ticker_throttle_create ();

void krad_ticker_start_at (krad_ticker_t *ticker, struct timespec start_time);
void krad_ticker_start (krad_ticker_t *ticker);
void krad_ticker_wait (krad_ticker_t *ticker);
void krad_ticker_throttle (krad_ticker_t *ticker, uint64_t ms);
