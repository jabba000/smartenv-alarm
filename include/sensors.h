#ifndef _SENSORS_H
#define _SENSORS_H

#include "context.h"

/**
 * Sensors initialization.
 */
void sensors_init(struct context *ctx);


/**
 * Get the temperature
 *
 * @return The temperature in °C.
 */
float get_temperature(struct context *ctx);


/**
 * Get the humidity
 *
 * @return Value of humidity as integer in the range from 0 (dry) to 100 (wet).
 */
int get_humidity(struct context *ctx);

#endif
