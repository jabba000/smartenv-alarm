#ifndef _CLOCK_H
#define _CLOCK_H

#include <Arduino.h>
#include "context.h"


struct dt {
    byte day;
    byte month;
    int year;
    byte hours;
    byte minutes;
    byte seconds;
};


/**
 * Initialize clock (RTC) module
 *
 */
void clock_init(struct context *ctx);


/**
 * Sets date on RTC
 */
void set_date(struct context *ctx, const byte day, const byte month, const int year);


/**
 * Sets time on RTC
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void set_time(struct context *ctx, const byte hours, const byte minutes, const byte seconds);


/**
 * Sets date and time on RTC
 *
 * @param day
 * @param month
 * @param year
 * @param hours
 * @param minutes
 * @param seconds
 */
void set_datetime(struct context *ctx, const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds);


/**
 * Gets day from RTC
 */
byte get_day(struct context *ctx);


/**
 * Gets month from RTC
 */
byte get_month(struct context *ctx);


/**
 * Gets year from RTC
 */
int get_year(struct context *ctx);


/**
 * Gets hour from RTC
 *
 * @return byte
 */
byte get_hours(struct context *ctx);


/**
 * Gets minute from RTC
 */
byte get_minutes(struct context *ctx);


/**
 * Gets second from RTC
 */
byte get_seconds(struct context *ctx);


/**
 * @brief
 *
 * @return struct dt
 */
struct dt now(struct context *ctx);

#endif
