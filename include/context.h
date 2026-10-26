#ifndef CONTEXT_H
#define CONTEXT_H

#include <I2C_LCD.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>
#include <DHT.h>

// all possible states of the alarm clock
enum State {
    STATE_NORMAL,           
    STATE_SET_TIME_HOUR,    
    STATE_SET_TIME_MIN,     
    STATE_SET_DATE_YEAR,  
    STATE_SET_DATE_MONTH,
    STATE_SET_DATE_DAY,   
    STATE_SET_ALARM_HOUR,  
    STATE_SET_ALARM_MIN,  
    STATE_ALARM_RINGING // buzzer is ringing
};

struct context {
    I2C_LCD *lcd;
    ThreeWire *myWire;
    RtcDS1302<ThreeWire> *rtc;
    DHT *dht;

    State state;

    // Current time
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    // Alarm settings
    int alarmHour;
    int alarmMinute;
    bool isAlarmActive;

    // DHT11
    float temperature;
    float humidity;

    unsigned long last_press;
    unsigned long last_update;
    unsigned long entry_time;
    unsigned long last_read;
    unsigned long last_toggle;
    int edit_mode;
    bool toggle;
};

#endif // CONTEXT_H
