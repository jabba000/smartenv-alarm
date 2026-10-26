#include "rtc_wrapper.h"
#include "config.h" 

void clock_init(struct context *ctx) {
    ctx->rtc->Begin();
    
    // Disable protection to allow change time by buttons 
    if (ctx->rtc->GetIsWriteProtected()) {
        ctx->rtc->SetIsWriteProtected(false);
    }
    
    // Start the RTC if it's not running 
    if (!ctx->rtc->GetIsRunning()) {
        ctx->rtc->SetIsRunning(true);
    }
}

void set_time(struct context *ctx, const byte hours, const byte minutes, const byte seconds) {
    RtcDateTime currentDate = ctx->rtc->GetDateTime();
    
    uint16_t y = currentDate.Year();
    uint8_t m = currentDate.Month();
    uint8_t d = currentDate.Day();

    // If data is broken set default date 
    if (!currentDate.IsValid()) {
        y = 2026; 
        m = 1; 
        d = 1;
    }

    RtcDateTime newTime(y, m, d, hours, minutes, seconds);
    ctx->rtc->SetDateTime(newTime);
}

void set_date(struct context *ctx, const byte day, const byte month, const int year) {
    RtcDateTime currentTime = ctx->rtc->GetDateTime();

    uint8_t h = currentTime.Hour();
    uint8_t min = currentTime.Minute();
    uint8_t s = currentTime.Second();

    // If time is broken set default time
    if (!currentTime.IsValid()) {
        h = 0;
        min = 0;
        s = 0;
    }

    RtcDateTime newTime(year, month, day, h, min, s);
    ctx->rtc->SetDateTime(newTime);
}

void set_datetime(struct context *ctx, const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds) {
    RtcDateTime newTime(year, month, day, hours, minutes, seconds);
    ctx->rtc->SetDateTime(newTime);
}

byte get_day(struct context *ctx) {
    return ctx->rtc->GetDateTime().Day();
}

byte get_month(struct context *ctx) {
    return ctx->rtc->GetDateTime().Month();
}

int get_year(struct context *ctx) {
    return ctx->rtc->GetDateTime().Year();
}

byte get_hours(struct context *ctx) {
    return ctx->rtc->GetDateTime().Hour();
}

byte get_minutes(struct context *ctx) {
    return ctx->rtc->GetDateTime().Minute();
}

byte get_seconds(struct context *ctx) {
    return ctx->rtc->GetDateTime().Second();
}

struct dt now(struct context *ctx) {
    RtcDateTime rtcNow = ctx->rtc->GetDateTime();
    struct dt current_dt;
    
    if (rtcNow.IsValid()) {
        current_dt.day = rtcNow.Day();
        current_dt.month = rtcNow.Month();
        current_dt.year = rtcNow.Year();
        current_dt.hours = rtcNow.Hour();
        current_dt.minutes = rtcNow.Minute();
        current_dt.seconds = rtcNow.Second();
    } else {
        // If RTC isnt working set default date and time
        current_dt.day = 1; current_dt.month = 1; current_dt.year = 2026;
        current_dt.hours = 0; current_dt.minutes = 0; current_dt.seconds = 0;
    }
    
    return current_dt;
}
