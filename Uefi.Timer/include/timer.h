#ifndef TIMER_H
#define TIMER_H

#include "uefi.h"

typedef struct {
    uint64_t start_time;
    uint64_t end_time;
    uint64_t elapsed_time;
    uint32_t frequency;
} Timer;

typedef struct {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t nanosecond;
    int16_t timezone;
    uint8_t daylight;
} DateTime;

int timer_init(void);
uint64_t timer_get_ticks(void);
uint64_t timer_get_frequency(void);
uint64_t timer_get_microseconds(void);
uint64_t timer_get_milliseconds(void);
void timer_delay_microseconds(uint32_t microseconds);
void timer_delay_milliseconds(uint32_t milliseconds);
void timer_delay_seconds(uint32_t seconds);

void timer_start(Timer* timer);
void timer_stop(Timer* timer);
uint64_t timer_get_elapsed_ticks(Timer* timer);
uint64_t timer_get_elapsed_microseconds(Timer* timer);
uint64_t timer_get_elapsed_milliseconds(Timer* timer);
void timer_reset(Timer* timer);

int datetime_get_current(DateTime* dt);
int datetime_set_current(const DateTime* dt);
void datetime_format_string(const DateTime* dt, char* buffer, size_t buffer_size);
uint64_t datetime_to_timestamp(const DateTime* dt);
void datetime_from_timestamp(uint64_t timestamp, DateTime* dt);

#define TIMER_TICKS_PER_SECOND 1000000

#endif
