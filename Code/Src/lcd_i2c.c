
#include "lcd_i2c.h"
#include "ctype.h"

/******************************Privated functions*******************************/

#define 	_LCD_ADDRESS	(0x27 << 1)
#define		_LCD_TIMEOUT	1000

static uint8_t 				_bl_status	= 0;
static I2C_HandleTypeDef*	_i2c_port	= NULL;

static HAL_StatusTypeDef LCD_SendByte(uint8_t mode, uint8_t data)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	/* BL: pos 3, EN: pos 2, RW: pos 1, RS: pos 0 */
	uint8_t data_arr[4] = 
	{
		(data & 0xF0) | (_bl_status << 3) | (1 << 2) | (mode << 0),
		(data & 0xF0) | (_bl_status << 3) | (mode << 0),
		(data << 4)   | (_bl_status << 3) | (1 << 2) | (mode << 0),
		(data << 4)   | (_bl_status << 3) | (mode << 0)
	};

	HAL_I2C_Master_Transmit(_i2c_port, _LCD_ADDRESS, data_arr, 4, _LCD_TIMEOUT);
	
	/* Wait to send */
	//HAL_Delay(1);
	return status;
}

/********************************************************************************/

void LCD_Init(I2C_HandleTypeDef* hi2c)
{
	/* Set I2C port */
	_i2c_port = hi2c;
	
	/* Wait power on */
	HAL_Delay(20);
	
	/* Initialize LCD 4 bits mode */
	LCD_SendByte(0, 0x33);
	LCD_SendByte(0, 0x32);
	LCD_SendByte(0, 0x28);
	/* Set mode */
	LCD_SendByte(0, AUTO_INCREASE_CURSOR);
	LCD_SendByte(0, DISPLAY_ON_CURSOR_OFF);
	LCD_SendByte(0, CLEAR_DISPLAY);
}

void LCD_DeInit(void)
{
	_bl_status = 0;
	_i2c_port = NULL;
}

void LCD_SetFunction(LCD_CommandTypeDef cmd)
{	
	LCD_SendByte(0, cmd);
}

void LCD_EnableBacklight(void)
{
	_bl_status = 1;
	
	LCD_SendByte(0, 0x06);	/* Dummy byte */	
}
	
void LCD_DisableBacklight(void)
{
	_bl_status = 0;
	
	LCD_SendByte(0, 0x06);	/* Dummy byte */
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
	uint8_t cmd = 0x80;
	
	switch (row)
	{
		case 0:
			cmd = 0x80;
			break;
		case 1:
			cmd = 0xC0;
			break;
		case 2:
			cmd = 0x94;
			break;
		case 3:
			cmd = 0xD4;
			break;
	}
	
	LCD_SendByte(0, cmd | col);
}

HAL_StatusTypeDef LCD_SendChar(char ch)
{
	return LCD_SendByte(1, ch);
}

HAL_StatusTypeDef LCD_SendString(char* str)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	uint8_t count = 0;
	
	while ( isprint(*(str+count)) )
	{
		status = LCD_SendByte(1, *(str+count));
		/* Check Error */
		if ( status != HAL_OK )
		{
			return status;
		}
		count++;
	}
	
	return status;
}

HAL_StatusTypeDef LCD_SendStringAt(char* str, uint8_t col, uint8_t row)
{
	LCD_SetCursor(col, row);
	return LCD_SendString(str);
}
