#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h"

/* Servo Angle Definitions */
#define SERVO_ANGLE_CLOSE    0
#define SERVO_ANGLE_OPEN     90

/**
  * @brief  Initialize Servo Motor PWM
  * @param  None
  * @retval None
  */
void Servo_Init(void);

/**
  * @brief  Set Servo Angle directly
  * @param  Angle Target angle (0 to 180)
  * @retval None
  */
void Servo_SetAngle(uint8_t Angle);

/**
  * @brief  Smoothly move Servo to target angle
  * @param  TargetAngle Target angle (0 to 180)
  * @param  StepDelayMs Delay in milliseconds per degree step
  * @retval None
  */
void Servo_SmoothSetAngle(uint8_t TargetAngle, uint16_t StepDelayMs);

/**
  * @brief  Get current Servo angle
  * @param  None
  * @retval Current angle
  */
uint8_t Servo_GetCurrentAngle(void);

#endif
