#include <Arduino.h>
#include <string.h>
#include "context.h"
#include "screens.h" 
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"

int main() {
    init();

    // Mem struct init
    struct context context;
    memset(&context, 0, sizeof(struct context)); // Clear all fields to zero

    // Hardware objects, no globals 
    I2C_LCD lcd(LCD_I2C_ADDRESS);
    ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
    RtcDS1302<ThreeWire> rtc(myWire);
    DHT dht(DHT_PIN, DHT11);

    context.lcd = &lcd;
    context.myWire = &myWire;
    context.rtc = &rtc;
    context.dht = &dht;

    enum screen screen = INIT_SCR;

    // 1. Pin setup
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
    pinMode(BTN4_PIN, INPUT_PULLUP);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RGB_RED_PIN, OUTPUT);
    pinMode(RGB_GREEN_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RGB_RED_PIN, LOW);
    digitalWrite(RGB_GREEN_PIN, LOW);

    // 2. Modules init 
    lcd_init(&context);
    clock_init(&context);
    sensors_init(&context);

    // 3. Main loop
    for(;;){
        switch(screen){
            case INIT_SCR:
                screen = init_screen(&context);
                break;

            case CLOCK_SCR:
                screen = clock_screen(&context);
                break;

            case SHOW_ENV_SCR:
                screen = show_env_screen(&context);
                break;

            case FACTORY_RESET_SCR:
                screen = factory_reset_screen(&context);
                break;

            case SHOW_DATE_SCR:
                screen = show_date_screen(&context);
                break;

            // Added states
            case ALARM_SCR:
                screen = alarm_screen(&context);
                break;

            case SET_TIME_SCR:
                screen = set_time_screen(&context);
                break;

            case RINGING_SCR:
                screen = ringing_screen(&context);
                break;

            default:
                // Reset 
                screen = INIT_SCR;
                break;
        }
    }
    
    return 0;
}
