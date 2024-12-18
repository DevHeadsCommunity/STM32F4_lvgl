#include "lcd.h"

void lcd_dc_pin_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= 1 << 18;
}
void lcd_rst_pin_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER |= 1 << 14;
    GPIOC->PUPDR |= 1 << 14;
}

/* Send short command to the LCD. This function shall wait until the transaction finishes. */
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    // LV_UNUSED(disp);
    /* DCX low (command) */
    LCD_DC_LOW();

    /* CS low */
    CS_Enable();

    /* send command */
    SPI_MasterTransmit(SPI1, (uint8_t *)cmd, cmd_size);

    /* DCX high (data) */
    LCD_DC_HIGH();

    /* for short data blocks we use polling transfer */
    SPI_MasterTransmit(SPI1, (uint8_t *)param, param_size);

    /* CS high */
    CS_Disable();
}

/* Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    /* DCX low (command) */
    LCD_DC_LOW();

    /* CS low */
    CS_Enable();

    /* send command */
    SPI_MasterTransmit(SPI1, (uint8_t *)cmd, cmd_size);

    /* DCX high (data) */
    LCD_DC_HIGH();

    /*Sending Data*/
    SPI_MasterTransmit(SPI1, (uint8_t *)param, param_size);

    /* CS high */
    CS_Disable();
    lv_disp_flush_ready(disp);
}