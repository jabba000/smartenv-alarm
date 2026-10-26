#include "sensors.h" 
#include "config.h"
#include <math.h>

void sensors_init(struct context *ctx) {
    ctx->dht->begin();
}

float get_temperature(struct context *ctx) {
    float t = ctx->dht->readTemperature();
    
    // If  the sensor didnt respond return 0 to avoid leaking NAN into the system   
    if (isnan(t)) return 0.0;
    
    return t;
}

int get_humidity(struct context *ctx) {
    float h = ctx->dht->readHumidity();
    
    if (isnan(h)) return 0;
    
    // float  => int
    return (int)h;
}
