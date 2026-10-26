#include "screens.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h> 

// Button debounce function
bool is_button_pressed(int pin, struct context *ctx) {
    if (digitalRead(pin) == LOW) {
        if (millis() - ctx->last_press > 250) { 
            ctx->last_press = millis();
            return true;
        }
    }
    return false;
}

// 1. Factory reset screen
enum screen factory_reset_screen(struct context *ctx) {
    lcd_clear(ctx);
    lcd_print_at(ctx, 0, 0, (char*)"Factory Reset");

    // Alarm reset
    ctx->alarmHour = 7;
    ctx->alarmMinute = 30;
    ctx->isAlarmActive = false;

    EEPROM.write(EEPROM_ALARM_HOUR_ADDR, ctx->alarmHour);
    EEPROM.write(EEPROM_ALARM_MIN_ADDR, ctx->alarmMinute);
    EEPROM.write(EEPROM_ALARM_ON_ADDR, 0);

    // RTC reset
    set_datetime(ctx, 1, 1, 2026, 0, 0, 0);

    delay(1500);

    // Block until btns are released
    ctx->last_press = millis();

    lcd_clear(ctx);
    ctx->last_update = 0; // Prepare for CLOCK_SCR
    return CLOCK_SCR;
}

// 2. Main screen with time and alarm status
enum screen clock_screen(struct context *ctx) {
    char buffer[17];

    if (digitalRead(BTN1_PIN) == LOW && digitalRead(BTN4_PIN) == LOW) {
        delay(100);
        if (digitalRead(BTN1_PIN) == LOW && digitalRead(BTN4_PIN) == LOW) {
            ctx->last_update = 0;
            return FACTORY_RESET_SCR;
        }
    }

    if (millis() - ctx->last_update > 1000) {
        struct dt current_time = now(ctx);
        ctx->hour = current_time.hours;
        ctx->minute = current_time.minutes;
        ctx->second = current_time.seconds;

        if (ctx->isAlarmActive && ctx->hour == ctx->alarmHour && ctx->minute == ctx->alarmMinute && ctx->second == 0) {
            lcd_clear(ctx);
            ctx->last_toggle = 0; // Prepare for RINGING_SCR
            ctx->toggle = false;
            return RINGING_SCR;
        }

        sprintf(buffer, "Time: %02d:%02d:%02d", ctx->hour, ctx->minute, ctx->second);
        lcd_print_at(ctx, 0, 0, buffer);
        
        sprintf(buffer, "Alarm: %s", ctx->isAlarmActive ? "ON " : "OFF");
        lcd_print_at(ctx, 1, 0, buffer);

        digitalWrite(RGB_RED_PIN, ctx->isAlarmActive ? HIGH : LOW);

        ctx->last_update = millis();
    }

    if (is_button_pressed(BTN1_PIN, ctx)) {
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for ALARM_SCR
        ctx->edit_mode = 0;
        return ALARM_SCR; 
    }
    if (is_button_pressed(BTN2_PIN, ctx)) {
        lcd_clear(ctx);
        ctx->entry_time = 0; // Prepare for SHOW_DATE_SCR
        return SHOW_DATE_SCR; 
    }
    if (is_button_pressed(BTN3_PIN, ctx)) {
        lcd_clear(ctx);
        ctx->entry_time = 0; // Prepare for SHOW_ENV_SCR
        ctx->last_read = 0;
        return SHOW_ENV_SCR; 
    }
    if (is_button_pressed(BTN4_PIN, ctx)) {
        ctx->isAlarmActive = !ctx->isAlarmActive;
        EEPROM.write(EEPROM_ALARM_ON_ADDR, ctx->isAlarmActive ? 1 : 0);
        ctx->last_update = 0; 
    }

    return CLOCK_SCR; 
}

// 3. Date screen
enum screen show_date_screen(struct context *ctx) {
    char buffer[17];
    
    // Init
    if (ctx->entry_time == 0) {
        ctx->entry_time = millis();
        if (ctx->entry_time == 0) ctx->entry_time = 1; 
        
        struct dt current_time = now(ctx);
        sprintf(buffer, "Date: %02d/%02d/%04d", current_time.day, current_time.month, current_time.year);
        lcd_print_at(ctx, 0, 0, buffer);
        lcd_print_at(ctx, 1, 0, (char*)"Push OK: EDIT ");
    }

    // Exit after 4 seconds of inactivity
    if (millis() - ctx->entry_time > 4000) {
        ctx->entry_time = 0; 
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    // Exit on button press
    if (is_button_pressed(BTN4_PIN, ctx)) {
        ctx->entry_time = 0; 
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for SET_TIME_SCR
        ctx->edit_mode = 0;
        return SET_TIME_SCR;
    }

    return SHOW_DATE_SCR;
}

// 4. DHT11 | Temperature and humidity screen
enum screen show_env_screen(struct context *ctx) {
    char buffer[17];

    // Init
    if (ctx->entry_time == 0) {
        ctx->entry_time = millis();
        if (ctx->entry_time == 0) ctx->entry_time = 1;
        ctx->last_read = 0; 
        lcd_clear(ctx);
        lcd_print_at(ctx, 0, 0, (char*)"Reading DHT11...");
    }

    // Get data every 2 seconds
    if (millis() - ctx->last_read > 2000) {
        float temp = get_temperature(ctx);
        int hum = get_humidity(ctx);

        sprintf(buffer, "Temp: %.1f C     ", temp);
        lcd_print_at(ctx, 0, 0, buffer);
        sprintf(buffer, "Hum:  %d %%         ", hum);
        lcd_print_at(ctx, 1, 0, buffer);

        ctx->last_read = millis();
    }

    // Exit after 6 seconds of inactivity
    if (millis() - ctx->entry_time > 6000 || is_button_pressed(BTN4_PIN, ctx)) {
        ctx->entry_time = 0;
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    return SHOW_ENV_SCR;
}

// 5. Alarm setting screen
enum screen alarm_screen(struct context *ctx) {
    char buffer[17];

    if (millis() - ctx->last_update > 200) {
        lcd_print_at(ctx, 0, 0, (char*)"Set Alarm Time: ");
        
        if (ctx->edit_mode == 0) sprintf(buffer, ">%02d< :  %02d   ", ctx->alarmHour, ctx->alarmMinute);
        else                sprintf(buffer, " %02d  : >%02d<  ", ctx->alarmHour, ctx->alarmMinute);
        
        lcd_print_at(ctx, 1, 0, buffer);
        ctx->last_update = millis();
    }

    if (is_button_pressed(BTN1_PIN, ctx)) {
        ctx->edit_mode = (ctx->edit_mode + 1) % 2; 
        ctx->last_update = 0;
    }
    if (is_button_pressed(BTN2_PIN, ctx)) {
        if (ctx->edit_mode == 0) ctx->alarmHour = (ctx->alarmHour + 1) % 24;
        else ctx->alarmMinute = (ctx->alarmMinute + 1) % 60;
        ctx->last_update = 0;
    }
    if (is_button_pressed(BTN3_PIN, ctx)) {
        if (ctx->edit_mode == 0) ctx->alarmHour = (ctx->alarmHour == 0) ? 23 : ctx->alarmHour - 1;
        else ctx->alarmMinute = (ctx->alarmMinute == 0) ? 59 : ctx->alarmMinute - 1;
        ctx->last_update = 0;
    }
    if (is_button_pressed(BTN4_PIN, ctx)) {
        EEPROM.write(EEPROM_ALARM_HOUR_ADDR, ctx->alarmHour);
        EEPROM.write(EEPROM_ALARM_MIN_ADDR, ctx->alarmMinute);
        lcd_clear(ctx);
        ctx->edit_mode = 0;  
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    return ALARM_SCR;
}

// 6. Ringing screen
enum screen ringing_screen(struct context *ctx) {
    if (millis() - ctx->last_toggle > 500) {
        ctx->toggle = !ctx->toggle;
        digitalWrite(BUZZER_PIN, ctx->toggle ? HIGH : LOW);
        digitalWrite(RGB_RED_PIN, ctx->toggle ? HIGH : LOW);
        
        if (ctx->toggle) {
            lcd_print_at(ctx, 0, 0, (char*)"   WAKE UP!     ");
        } else {
            lcd_clear(ctx);
        }
        ctx->last_toggle = millis();
    }

    if (is_button_pressed(BTN3_PIN, ctx)) {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RGB_RED_PIN, LOW);
        
        ctx->alarmMinute += 1;
        if (ctx->alarmMinute >= 60) {
            ctx->alarmMinute -= 60;
            ctx->alarmHour = (ctx->alarmHour + 1) % 24;
        }
        
        EEPROM.write(EEPROM_ALARM_HOUR_ADDR, ctx->alarmHour);
        EEPROM.write(EEPROM_ALARM_MIN_ADDR, ctx->alarmMinute);
        
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    if (is_button_pressed(BTN4_PIN, ctx)) {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RGB_RED_PIN, LOW);
        digitalWrite(RGB_GREEN_PIN, HIGH);
        delay(100);
        digitalWrite(RGB_GREEN_PIN, LOW);
        delay(100);
        digitalWrite(RGB_GREEN_PIN, HIGH);
        delay(100);
        digitalWrite(RGB_GREEN_PIN, LOW);
        delay(100);
        digitalWrite(RGB_GREEN_PIN, HIGH);
        delay(100);
        digitalWrite(RGB_GREEN_PIN, LOW);
        lcd_clear(ctx);
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    return RINGING_SCR;
}

// 7. Current time and date setting screen
enum screen set_time_screen(struct context *ctx) {
    char buffer[17];

    // Init - get data from RTC on first entry
    if (ctx->last_update == 0) {
        struct dt current_dt = now(ctx);
        ctx->hour = current_dt.hours;
        ctx->minute = current_dt.minutes;
        ctx->day = current_dt.day;
        ctx->month = current_dt.month;
        ctx->year = current_dt.year;
        ctx->last_update = millis();
    }

    if (millis() - ctx->last_update > 200) {
        if (ctx->edit_mode < 2) {
            lcd_print_at(ctx, 0, 0, (char*)"Set RTC Time:   ");
            if (ctx->edit_mode == 0) sprintf(buffer, ">%02d< :  %02d      ", ctx->hour, ctx->minute);
            else                sprintf(buffer, " %02d  : >%02d<     ", ctx->hour, ctx->minute);
        } else {
            lcd_print_at(ctx, 0, 0, (char*)"Set RTC Date:   ");
            if (ctx->edit_mode == 2)     sprintf(buffer, ">%02d</ %02d /%04d", ctx->day, ctx->month, ctx->year);
            else if (ctx->edit_mode == 3) sprintf(buffer, " %02d />%02d</%04d", ctx->day, ctx->month, ctx->year);
            else                     sprintf(buffer, " %02d / %02d />%04d<", ctx->day, ctx->month, ctx->year);
        }

        lcd_print_at(ctx, 1, 0, buffer);
        ctx->last_update = millis();
    }

    if (is_button_pressed(BTN1_PIN, ctx)) {
        ctx->edit_mode = (ctx->edit_mode + 1) % 5;
        ctx->last_update = millis(); 
    }

    if (is_button_pressed(BTN2_PIN, ctx)) {
        if (ctx->edit_mode == 0) ctx->hour = (ctx->hour + 1) % 24;
        else if (ctx->edit_mode == 1) ctx->minute = (ctx->minute + 1) % 60;
        else if (ctx->edit_mode == 2) ctx->day = (ctx->day % 31) + 1;
        else if (ctx->edit_mode == 3) ctx->month = (ctx->month % 12) + 1;
        else if (ctx->edit_mode == 4) ctx->year = (ctx->year >= 2099) ? 2000 : ctx->year + 1;
        ctx->last_update = millis();
    }

    if (is_button_pressed(BTN3_PIN, ctx)) {
        if (ctx->edit_mode == 0) ctx->hour = (ctx->hour == 0) ? 23 : ctx->hour - 1;
        else if (ctx->edit_mode == 1) ctx->minute = (ctx->minute == 0) ? 59 : ctx->minute - 1;
        else if (ctx->edit_mode == 2) ctx->day = (ctx->day == 1) ? 31 : ctx->day - 1;
        else if (ctx->edit_mode == 3) ctx->month = (ctx->month == 1) ? 12 : ctx->month - 1;
        else if (ctx->edit_mode == 4) ctx->year = (ctx->year <= 2000) ? 2099 : ctx->year - 1;
        ctx->last_update = millis();
    }

    if (is_button_pressed(BTN4_PIN, ctx)) {
        set_datetime(ctx, ctx->day, ctx->month, ctx->year, ctx->hour, ctx->minute, 0);
        lcd_clear(ctx);
        ctx->edit_mode = 0;   
        ctx->last_update = 0; // Prepare for CLOCK_SCR
        return CLOCK_SCR;
    }

    return SET_TIME_SCR;
}
