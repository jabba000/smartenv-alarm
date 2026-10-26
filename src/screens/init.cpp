#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#include "config.h"
#include "context.h"
#include "screens.h"
#include "lcd_wrapper.h"

enum screen init_screen(struct context *ctx){
    init();

    // Serial init 
    Serial.begin(BAUD_RATE);
    while(!Serial);
    Serial.println("> Init Screen");

    // EEPROM logic
    lcd_clear(ctx);
    lcd_print_at(ctx, 0, 0, (char*)"Booting...");
    
    // Read raw data from EEPROM
    byte h = EEPROM.read(EEPROM_ALARM_HOUR_ADDR);
    byte m = EEPROM.read(EEPROM_ALARM_MIN_ADDR);
    byte a = EEPROM.read(EEPROM_ALARM_ON_ADDR);

    // If EEPROM has trash, reset to default values
    if (h > 23 || m > 59 || a > 1) return FACTORY_RESET_SCR;
    

    // Save data to context
    ctx->alarmHour = h;
    ctx->alarmMinute = m;
    ctx->isAlarmActive = (a == 1);

    delay(1000); 
    lcd_clear(ctx);
    
    return CLOCK_SCR;
}
