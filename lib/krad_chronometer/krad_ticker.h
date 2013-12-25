#include <time.h>

#include "krad_system.h"

typedef struct kr_ticker kr_ticker;
typedef struct kr_ticker krad_ticker_t;

struct kr_ticker {
  struct timespec start_time;
  struct timespec wakeup_time;
  uint64_t period_time_ns;
  uint64_t total_periods;
  uint64_t total_ms;
};

struct timespec timespec_add_ms(struct timespec ts, uint64_t ms);
struct timespec timespec_add_ns(struct timespec ts, uint64_t ns);

void krad_ticker_destroy(kr_ticker *ticker);
krad_ticker_t *krad_ticker_create(int numerator, int denominator);
krad_ticker_t *krad_ticker_throttle_create();

void krad_ticker_start_at(kr_ticker *ticker, struct timespec start_time);
void krad_ticker_start(kr_ticker *ticker);
void krad_ticker_wait(kr_ticker *ticker);
void krad_ticker_throttle(kr_ticker *ticker, uint64_t ms);
