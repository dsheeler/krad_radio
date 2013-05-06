#include "krad_timer.h"

static inline uint64_t ts_to_ms (struct timespec ts) {
  return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

krad_timer_t *krad_timer_create() {
  return krad_timer_create_with_name ("");
}

krad_timer_t *krad_timer_create_with_name (const char *name) {
  krad_timer_t *krad_timer = calloc(1, sizeof(krad_timer_t));
  krad_timer->name = name;
  return krad_timer;
}

void krad_timer_status (krad_timer_t *krad_timer) {
  printk ("Krad Radio: %s timer at %"PRIu64"ms",
          krad_timer->name,
          krad_timer_sample_duration_ms (krad_timer));
}

void krad_timer_start (krad_timer_t *krad_timer) {
  clock_gettime ( CLOCK_MONOTONIC, &krad_timer->start);
  krad_timer->started = 1;
}

int32_t krad_timer_started (krad_timer_t *timer) {
  return timer->started;
}

void krad_timer_finish (krad_timer_t *krad_timer) {
  clock_gettime ( CLOCK_MONOTONIC, &krad_timer->finish);
}

uint64_t krad_timer_sample_duration_ms (krad_timer_t *krad_timer) {
  clock_gettime ( CLOCK_MONOTONIC, &krad_timer->sample );
  return ts_to_ms (krad_timer->sample) - ts_to_ms (krad_timer->start);
}

uint64_t krad_timer_current_ms (krad_timer_t *krad_timer) {
  if (krad_timer->started == 0) {
    return 0;
  }
  return krad_timer_sample_duration_ms (krad_timer);
}

uint64_t krad_timer_duration_ms (krad_timer_t *krad_timer) {
  return ts_to_ms (krad_timer->finish) - ts_to_ms (krad_timer->start);
}

void krad_timer_destroy(krad_timer_t *krad_timer) {
  free (krad_timer);
}
