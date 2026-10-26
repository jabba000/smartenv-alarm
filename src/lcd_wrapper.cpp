#include "lcd_wrapper.h"
#include "config.h"
#include <Wire.h>

void lcd_init(struct context *ctx) {
    Wire.begin();
    ctx->lcd->begin(LCD_COLS, LCD_ROWS);
    
    // Enanle backlight by default 
    ctx->lcd->setBacklight(true); 
}

void lcd_clear(struct context *ctx) {
    ctx->lcd->clear();
}

void lcd_set_cursor(struct context *ctx, int y, int x) {
    // Reverse x & y because arduino LCD lib uses col, row 
    ctx->lcd->setCursor(x, y);
}

void lcd_print(struct context *ctx, char* text) {
    ctx->lcd->print(text);
}

void lcd_print_at(struct context *ctx, int y, int x, char* text) {
    ctx->lcd->setCursor(x, y);
    ctx->lcd->print(text);
}

void lcd_backlight(struct context *ctx, bool state) {
    ctx->lcd->setBacklight(state);
}
