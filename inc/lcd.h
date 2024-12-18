#ifndef __INC_LCD_H__
#define __INC_LCD_H__
#include "spi.h"
#include "lvgl.h"

#define LCD_RS_HIGH() (GPIOC->BSRR |= (1 << 7))
#define LCD_RS_LOW() (GPIOC->BSRR |= (1 << (16 + 7)))

#define LCD_DC_HIGH() (GPIOA->BSRR |= (1 << 9U))
#define LCD_DC_LOW() (GPIOA->BSRR |= (1 << (16 + 9U)))

void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);
void lcd_rst_pin_init();
void lcd_dc_pin_init();

#endif