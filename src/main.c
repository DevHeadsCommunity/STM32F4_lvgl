#include "stm32f446xx.h"
#include "lvgl.h"
#include "lcd.h"
#include "timer.h"
#include "lv_font.h"

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 280
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
// Display buffer for partial rendering
static uint8_t buf1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 5 * BYTE_PER_PIXEL];

void draw_hum_arc()
{
    lv_obj_t *arc = lv_arc_create(lv_screen_active());
    lv_obj_t *label = lv_label_create(arc);

    static lv_style_t style;
    lv_label_set_text(label, "70");
    lv_style_set_text_font(&style, &lv_font_montserrat_28);
    lv_obj_center(label);
    lv_obj_add_style(label, &style, 0); // Apply the style to the label

    /*Create an Arc*/
    lv_obj_set_size(arc, 120, 120);

    lv_arc_set_rotation(arc, 135); // Completed

    lv_arc_set_bg_angles(arc, 0, 270);// FUll arc

    lv_arc_set_value(arc, 70);
    
    lv_obj_set_x(arc, 60);
    lv_obj_set_y(arc, 60);
}

lv_obj_t *btn1;

void check_physical_button()
{
    static uint8_t prev_state = 1;                             // Assume button is not pressed initially
    uint8_t curr_state = (GPIOC->IDR & GPIO_IDR_ID13) ? 1 : 0; // Read PA0 state

    if (prev_state == 1 && curr_state == 0) // Detect button press (falling edge)
    {
        // Toggle the LVGL button state
        if (lv_obj_has_flag(btn1, LV_OBJ_FLAG_CHECKABLE))
        {
            bool checked = lv_obj_has_state(btn1, LV_STATE_CHECKED);
            if (checked)
                lv_obj_clear_state(btn1, LV_STATE_CHECKED);
            else
                lv_obj_add_state(btn1, LV_STATE_CHECKED);
        }
    }

    prev_state = curr_state;
}

void add_btn()
{
    // Adding Btn
    btn1 = lv_button_create(lv_screen_active());
    lv_obj_set_x(btn1, 60);
    lv_obj_set_y(btn1, 220);
    lv_obj_set_size(btn1, 120, 40);

    // Adding Label
    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);

    // Adding Style
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_20); // Set the font for the style
    lv_style_set_text_color(&style, lv_color_hex(0x000000));
    lv_obj_add_style(label, &style, 0); // Apply the style to the label

    // Adding flag it to it
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);

}

void usr_btn_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    GPIOC->MODER &= ~(3U << 26);

    GPIOC->PUPDR |= 1 << 26;
}

int main()
{
    // Initialize Timer and SPI
    TimerInit();
    TIM2_Init();
    SPIGPIOConfig();
    SPIConfig();
    lcd_dc_pin_init(); // Data or Command 
    lcd_rst_pin_init();
    usr_btn_init();

    LCD_RS_LOW();
    DelayMS(10);
    LCD_RS_HIGH();
    DelayMS(10);

    // Initialize LVGL
    lv_init();

    // Create LVGL display driver
    lv_display_t *display = lv_st7789_create(MY_DISP_HOR_RES, MY_DISP_VER_RES, LV_LCD_FLAG_NONE,
                                             my_lcd_send_cmd, my_lcd_send_color);

    // Set display buffers

    lv_display_set_buffers(display, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_st7789_set_gamma_curve(display, 4);

    draw_hum_arc();

    add_btn();

    while (1)
    {
        // Periodically handle LVGL tasks
        lv_timer_handler();
        check_physical_button();
    }

    return 0;
}

void TIM2_IRQHandler()
{
    // Check if the update interrupt flag is set
    if (TIM2->SR & TIM_SR_UIF)
    {
        // Clear the update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF;

        // Increment LVGL tick by 1ms
        lv_tick_inc(1);
    }
}
