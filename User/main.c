#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "HC_SR04.h"
#include "Servo.h"
#include "Buzzer_LED.h"
#include "Key.h"

#define MODE_AUTO   0
#define MODE_MANUAL 1

#define BIN_EMPTY_DEPTH  30.0f
#define BIN_FULL_DEPTH   5.0f

#define HUMAN_DETECT_DIST_CM 20.0f
#define OVERFLOW_PERCENT_THRESHOLD 85

static uint8_t g_system_mode = MODE_AUTO;
static uint8_t g_lid_state = 0;
static uint32_t g_open_timer_ms = 0;

void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    OLED_Init();
    HC_SR04_Init();
    Servo_Init();
    Buzzer_LED_Init();
    Key_Init();
    
    Servo_SetAngle(0);
    g_lid_state = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Smart Trash Can");
    OLED_ShowString(2, 1, "System Init...");
    Delay_ms(1000);
    OLED_Clear();
}

uint8_t Calculate_Capacity(float depth_cm)
{
    if (depth_cm >= BIN_EMPTY_DEPTH) {
        return 0;
    }
    if (depth_cm <= BIN_FULL_DEPTH) {
        return 100;
    }
    
    float used_space = BIN_EMPTY_DEPTH - depth_cm;
    float total_space = BIN_EMPTY_DEPTH - BIN_FULL_DEPTH;
    float percent = (used_space / total_space) * 100.0f;
    
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;
    
    return (uint8_t)percent;
}

void Update_OLED_Display(float human_dist, float bin_depth, uint8_t capacity, uint8_t mode, uint8_t lid)
{
    if (mode == MODE_AUTO) {
        OLED_ShowString(1, 1, "Mode: AUTO ");
    } else {
        OLED_ShowString(1, 1, "Mode: MANU ");
    }
    
    if (lid == 1) {
        OLED_ShowString(1, 12, "OPEN ");
    } else {
        OLED_ShowString(1, 12, "CLOSE");
    }
    
    OLED_ShowString(2, 1, "H-Dist: ");
    if (human_dist > 400.0f || human_dist < 0.0f) {
        OLED_ShowString(2, 9, "ERR  cm");
    } else {
        OLED_ShowNum(2, 9, (uint32_t)human_dist, 3);
        OLED_ShowString(2, 12, "cm ");
    }
    
    OLED_ShowString(3, 1, "B-Dist: ");
    if (bin_depth > 400.0f || bin_depth < 0.0f) {
        OLED_ShowString(3, 9, "ERR  cm");
    } else {
        OLED_ShowNum(3, 9, (uint32_t)bin_depth, 3);
        OLED_ShowString(3, 12, "cm ");
    }
    
    OLED_ShowString(4, 1, "Cap: ");
    OLED_ShowNum(4, 6, capacity, 3);
    OLED_ShowString(4, 9, "% ");
    
    if (capacity >= OVERFLOW_PERCENT_THRESHOLD) {
        OLED_ShowString(4, 12, "[FULL]");
    } else {
        OLED_ShowString(4, 12, "[OK]  ");
    }
}

int main(void)
{
    float human_dist_cm = 0.0f;
    float bin_depth_cm = 0.0f;
    uint8_t capacity_percent = 0;
    uint8_t key_val = 0;
    uint8_t ir_blocked = 0;
    
    System_Init();
    
    while (1)
    {
        key_val = Key_GetNum();
        if (key_val == 1) {
            g_system_mode = (g_system_mode == MODE_AUTO) ? MODE_MANUAL : MODE_AUTO;
            Buzzer_Beep(50);
        } else if (key_val == 2) {
            if (g_lid_state == 0) {
                Servo_SmoothSetAngle(90);
                g_lid_state = 1;
            } else {
                Servo_SmoothSetAngle(0);
                g_lid_state = 0;
            }
            Buzzer_Beep(50);
        }
        
        human_dist_cm = HC_SR04_GetHumanDistanceFilter();
        bin_depth_cm = HC_SR04_GetBinDepthFilter();
        
        capacity_percent = Calculate_Capacity(bin_depth_cm);
        
        ir_blocked = Key_GetIRState();
        
        if (g_system_mode == MODE_AUTO)
        {
            if (human_dist_cm > 0.0f && human_dist_cm < HUMAN_DETECT_DIST_CM) {
                if (g_lid_state == 0) {
                    Servo_SmoothSetAngle(90);
                    g_lid_state = 1;
                }
                g_open_timer_ms = 0;
            } else {
                if (g_lid_state == 1) {
                    if (ir_blocked == 1) {
                        g_open_timer_ms = 0;
                        LED_Red_On();
                    } else {
                        LED_Red_Off();
                        g_open_timer_ms += 100;
                        if (g_open_timer_ms >= 3000) {
                            Servo_SmoothSetAngle(0);
                            g_lid_state = 0;
                            g_open_timer_ms = 0;
                        }
                    }
                }
            }
        }
        
        if (capacity_percent >= OVERFLOW_PERCENT_THRESHOLD) {
            LED_Red_On();
            LED_Green_Off();
            Buzzer_Alarm_Pattern();
        } else {
            LED_Green_On();
            if (ir_blocked == 0) {
                LED_Red_Off();
            }
            Buzzer_Off();
        }
        
        Update_OLED_Display(human_dist_cm, bin_depth_cm, capacity_percent, g_system_mode, g_lid_state);
        
        Delay_ms(100);
    }
}
