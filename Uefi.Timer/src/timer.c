#include "timer.h"
#include "uefi_lib.h"
#include "../Uefi.StdLib/include/stdlib.h"
#include "../Uefi.StdLib/include/string.h"

static EFI_RUNTIME_SERVICES* g_rt = NULL;
static uint64_t g_timer_frequency = 0;

int timer_init(void) {
    if (!gST) return 0;
    
    g_rt = gST->RuntimeServices;
    if (!g_rt) return 0;
    
    g_timer_frequency = timer_get_frequency();
    return g_timer_frequency > 0;
}

uint64_t timer_get_ticks(void) {
    if (!g_rt) return 0;
    
    EFI_TIME time;
    EFI_STATUS status = g_rt->GetTime(&time, NULL);
    if (EFI_ERROR(status)) return 0;
    
    return (uint64_t)time.Year * 365 * 24 * 60 * 60 +
           (uint64_t)time.Month * 30 * 24 * 60 * 60 +
           (uint64_t)time.Day * 24 * 60 * 60 +
           (uint64_t)time.Hour * 60 * 60 +
           (uint64_t)time.Minute * 60 +
           (uint64_t)time.Second;
}

uint64_t timer_get_frequency(void) {
    if (g_timer_frequency > 0) return g_timer_frequency;
    
    if (!gBS) return 0;
    
    EFI_GUID stall_guid = {0x4B8935, 0x4A, 0x11, {0x8B, 0x4C, 0x89, 0x34, 0x01, 0x09, 0x54, 0x3A}};
    
    uint64_t start = timer_get_ticks();
    gBS->Stall(1000000); // 1 second
    uint64_t end = timer_get_ticks();
    
    g_timer_frequency = end - start;
    if (g_timer_frequency == 0) {
        g_timer_frequency = TIMER_TICKS_PER_SECOND;
    }
    
    return g_timer_frequency;
}

uint64_t timer_get_microseconds(void) {
    uint64_t ticks = timer_get_ticks();
    uint64_t freq = timer_get_frequency();
    
    if (freq == 0) return 0;
    
    return (ticks * 1000000) / freq;
}

uint64_t timer_get_milliseconds(void) {
    return timer_get_microseconds() / 1000;
}

void timer_delay_microseconds(uint32_t microseconds) {
    if (gBS) {
        gBS->Stall(microseconds);
    } else {
        volatile uint32_t count = microseconds * 100;
        while (count--) {
            __asm__ volatile ("nop");
        }
    }
}

void timer_delay_milliseconds(uint32_t milliseconds) {
    timer_delay_microseconds(milliseconds * 1000);
}

void timer_delay_seconds(uint32_t seconds) {
    timer_delay_milliseconds(seconds * 1000);
}

void timer_start(Timer* timer) {
    if (!timer) return;
    
    timer->start_time = timer_get_ticks();
    timer->end_time = 0;
    timer->elapsed_time = 0;
    timer->frequency = timer_get_frequency();
}

void timer_stop(Timer* timer) {
    if (!timer) return;
    
    timer->end_time = timer_get_ticks();
    timer->elapsed_time = timer->end_time - timer->start_time;
}

uint64_t timer_get_elapsed_ticks(Timer* timer) {
    if (!timer) return 0;
    
    if (timer->end_time == 0) {
        return timer_get_ticks() - timer->start_time;
    }
    
    return timer->elapsed_time;
}

uint64_t timer_get_elapsed_microseconds(Timer* timer) {
    uint64_t ticks = timer_get_elapsed_ticks(timer);
    uint64_t freq = timer->frequency;
    
    if (freq == 0) return 0;
    
    return (ticks * 1000000) / freq;
}

uint64_t timer_get_elapsed_milliseconds(Timer* timer) {
    return timer_get_elapsed_microseconds(timer) / 1000;
}

void timer_reset(Timer* timer) {
    if (!timer) return;
    
    timer->start_time = 0;
    timer->end_time = 0;
    timer->elapsed_time = 0;
    timer->frequency = timer_get_frequency();
}

static int is_leap_year(uint32_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint32_t days_in_month(uint32_t month, uint32_t year) {
    static const uint32_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    
    return days[month - 1];
}

int datetime_get_current(DateTime* dt) {
    if (!dt || !g_rt) return 0;
    
    EFI_TIME efi_time;
    EFI_STATUS status = g_rt->GetTime(&efi_time, NULL);
    if (EFI_ERROR(status)) return 0;
    
    dt->year = efi_time.Year;
    dt->month = efi_time.Month;
    dt->day = efi_time.Day;
    dt->hour = efi_time.Hour;
    dt->minute = efi_time.Minute;
    dt->second = efi_time.Second;
    dt->nanosecond = efi_time.Nanosecond;
    dt->timezone = efi_time.TimeZone;
    dt->daylight = efi_time.Daylight;
    
    return 1;
}

int datetime_set_current(const DateTime* dt) {
    if (!dt || !g_rt) return 0;
    
    EFI_TIME efi_time;
    efi_time.Year = dt->year;
    efi_time.Month = dt->month;
    efi_time.Day = dt->day;
    efi_time.Hour = dt->hour;
    efi_time.Minute = dt->minute;
    efi_time.Second = dt->second;
    efi_time.Nanosecond = dt->nanosecond;
    efi_time.TimeZone = dt->timezone;
    efi_time.Daylight = dt->daylight;
    
    EFI_STATUS status = g_rt->SetTime(&efi_time);
    return !EFI_ERROR(status);
}

void datetime_format_string(const DateTime* dt, char* buffer, size_t buffer_size) {
    if (!dt || !buffer || buffer_size < 20) return;
    
    snprintf(buffer, buffer_size, "%04d-%02d-%02d %02d:%02d:%02d",
             dt->year, dt->month, dt->day,
             dt->hour, dt->minute, dt->second);
}

uint64_t datetime_to_timestamp(const DateTime* dt) {
    if (!dt) return 0;
    
    uint64_t timestamp = 0;
    
    for (uint32_t year = 1970; year < dt->year; year++) {
        timestamp += is_leap_year(year) ? 366 * 24 * 60 * 60 : 365 * 24 * 60 * 60;
    }
    
    for (uint32_t month = 1; month < dt->month; month++) {
        timestamp += days_in_month(month, dt->year) * 24 * 60 * 60;
    }
    
    timestamp += (dt->day - 1) * 24 * 60 * 60;
    timestamp += dt->hour * 60 * 60;
    timestamp += dt->minute * 60;
    timestamp += dt->second;
    
    return timestamp;
}

void datetime_from_timestamp(uint64_t timestamp, DateTime* dt) {
    if (!dt) return;
    
    memset(dt, 0, sizeof(DateTime));
    
    uint32_t days = timestamp / (24 * 60 * 60);
    uint32_t seconds = timestamp % (24 * 60 * 60);
    
    dt->year = 1970;
    while (days >= (is_leap_year(dt->year) ? 366 : 365)) {
        days -= is_leap_year(dt->year) ? 366 : 365;
        dt->year++;
    }
    
    dt->month = 1;
    while (days >= days_in_month(dt->month, dt->year)) {
        days -= days_in_month(dt->month, dt->year);
        dt->month++;
    }
    
    dt->day = days + 1;
    dt->hour = seconds / 3600;
    dt->minute = (seconds % 3600) / 60;
    dt->second = seconds % 60;
}
