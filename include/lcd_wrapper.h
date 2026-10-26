#ifndef _LCD_H
#define _LCD_H

#include "context.h"

/**
 * Initialize LCD
 *
 * Everything you need to initialize your LCD display, place here.
 * This function should be called in function setup().
 */
void lcd_init(struct context *ctx);


/**
 * Clears the LCD display
 *
 * Clears content of the LCD display. After the call is display empty.
 */
void lcd_clear(struct context *ctx);


/**
 * Set cursor to given posititon
 *
 * Sets cursor to given position based on y and x coordinates.
 * @param y the row
 * @param x the column
 */
void lcd_set_cursor(struct context *ctx, int y, int x);


/**
 * Print text on LCD display
 *
 * Prints the givent text on the display on the current
 * cursor position.
 * @param text the text to print
 */
void lcd_print(struct context *ctx, char* text);


/**
 * Set cursor to given positition and print text
 *
 * Sets the cursor to given position based on y and x coorindates
 * and then prints the given text.
 * @param y the row
 * @param x the column
 * @param text the text to print
 */
void lcd_print_at(struct context *ctx, int y, int x, char* text);


/**
 * Turns LCD backlight on/off.
 *
 *  Turns LCD backlight on or off based on the `state` parameter:
 *  - if set to true, backlight will be turned on
 *  - if set to false, backlight will be turned off
 *
 * @param state state of the backlight
 */
void lcd_backlight(struct context *ctx, bool state);

#endif
