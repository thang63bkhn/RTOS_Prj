
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


typedef enum
{
	CLEAR_DISPLAY 				= 0x01,
	RETURN_HOME 				= 0x02,
	AUTO_INCREASE_CURSOR 		= 0x06,
	DISPLAY_OFF 				= 0x08,
	DISPLAY_ON_CURSOR_OFF 		= 0x0C,
	DISPLAY_ON_CURSOR_ON 		= 0x0E,
	DISPLAY_ON_CURSOR_BLINK 	= 0x0F,
	SHIFT_SURSOR_TO_LEFT 		= 0x10,
	SHIFT_SURSOR_TO_RIGHT 		= 0x14,
	SCROLL_DISPLAY_TO_LEFT 		= 0x18,
	SCROLL_DISPLAY_TO_RIGHT 	= 0x1C
} LCD_CommandTypeDef;

/*============== Functions prototypes ==============*/
void LCD_Init(I2C_HandleTypeDef* hi2c);
void LCD_DeInit(void);

void LCD_SetFunction(LCD_CommandTypeDef cmd);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_EnableBacklight(void);
void LCD_DisableBacklight(void);

HAL_StatusTypeDef LCD_SendChar(char ch);
HAL_StatusTypeDef LCD_SendString(char* str);
HAL_StatusTypeDef LCD_SendStringAt(char* str, uint8_t col, uint8_t row);
/*==================================================*/

#ifdef __cplusplus
}
#endif

#endif /* __LCD_I2C_H */
