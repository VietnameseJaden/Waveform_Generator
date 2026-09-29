/*
 * Driver.c
 *
 *  Created on: Sep 10, 2026
 *      Author: thisf
 */
#include "main.h"
#include "Display.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

uint8_t FRAME_BUFFER[SIZE] = {0};


void position(int x, int y, bool state)
{
	if((x < 0) || (x >= WIDTH) ||
	(y < 0) || (y >= HEIGHT))
	{
	return;
	}

	int position = (WIDTH*(y>>3)) + x;

	if(state)
	{
		FRAME_BUFFER[position] |= (1U << (y & 7U));
	}
	else
	{
		FRAME_BUFFER[position] &= ~(1U << (y & 7U));
	}
}

bool get_position(int x, int y)
{
	int position = (WIDTH *(y>>3)) +x;
	return FRAME_BUFFER[position] & (1U << (y & 7U));
}

void fill_row(int y, bool state)
{
	int position = (WIDTH*y) >> 3;

	if(state)
	{
		FRAME_BUFFER[position] |= (0b11111111U);
	}
	else
	{
		FRAME_BUFFER[position] &= ~(0b11111111U);
	}
}

void draw_image(int x, int y, int height, int width, const uint8_t *image)
{
	int size = (height*width) >> 3;
	int originalx = x;
	int originaly = y;

	uint8_t mask;

	for(int i = 0; i<size; i++)
	{
		for(int j = 0; j<8; j++)
		{
			mask = 0x80U >> j;
			position(x, y, image[i] & mask);
			x++;

			if((x - originalx) == width)
			{
				y++;
				x = originalx;
			}
			if((y - originaly) >= height)
			{
				return;
			}

		}
	}
	return;
}

void draw_wave(int y)
{
	int converter = 63 - ((y*63)/4095);
	static int x = 0;
	static int originaly = 32;
	static bool first = true;

	if(first)
	{
		position(x, converter, true);
		first = false;
	}
	else
	{
		draw_line(x - 1, originaly, x, y);
	}
	originaly = converter;
	x++;

	if(x >= WIDTH)
	{
		x = 0;
		first = true;
		memset(FRAME_BUFFER, 0, SIZE);
	}
}

void clearbuffer()
{
	for (int i = 0; i < SIZE; i++)
	{
		FRAME_BUFFER[i] = 0;
	}
}

void draw_line(int lastx, int lasty, int x1, int y1)
{
	int interpolation = (y1 - lasty) / (x1 - lastx);

	if(lasty < y1) // (1,10) and (2, 20) and (3,10) and (4,20)
	{
		for(int y = lasty; y <= y1; y++)
		{
			position(lastx, y, true);
		}
	}
	else
	{
		for(int y = y1; y<= lasty; y++)
		{
			position(x1, y, true);
		}
	}
	position(x1, y1, true);
}



void OLED_GPIO_Init(void)
	   {
	   	/* Enable GPIOA, GPIOB and GPIOC clocks */
	   	RCC->IOPENR |= (1UL << 0);  /* GPIOA */
	   	RCC->IOPENR |= (1UL << 1);  /* GPIOB */
	   	RCC->IOPENR |= (1UL << 2);  /* GPIOC */

	   	(void)RCC->IOPENR;

	   	/* PA7 = MOSI */
	   	GPIOA->MODER &= ~(3UL << 14);
	   	GPIOA->MODER |=  (1UL << 14);

	   	/* PA8 = RESET */
	   	GPIOA->MODER &= ~(3UL << 16);
	   	GPIOA->MODER |=  (1UL << 16);

	   	/* PB8 = CLK */
	   	GPIOB->MODER &= ~(3UL << 16);
	   	GPIOB->MODER |=  (1UL << 16);

	   	/* PB4 = CS */
	   	GPIOB->MODER &= ~(3UL << 8);
	   	GPIOB->MODER |=  (1UL << 8);

	   	/* PC8 = DC */
	   	GPIOC->MODER &= ~(3UL << 16);
	   	GPIOC->MODER |=  (1UL << 16);

	   	/* Start CLK low */
	   	GPIOB->BSRR = (1UL << (8U + 16U));

	   	/* Start CS high */
	   	GPIOB->BSRR = (1UL << 4U);

	   	/* Start DC low */
	   	GPIOC->BSRR = (1UL << (8U + 16U));

	   	/* Start RESET high */
	   	GPIOA->BSRR = (1UL << 8U);
	   }

	   void OLED_Send_Byte(uint8_t data)
	   {
	   	for(int bit = 7; bit >= 0; bit--)
	   	{
	   		/* CLK low */
	   		GPIOB->BSRR = (1UL << (8U + 16U));

	   		/* Put the current bit onto MOSI, PA7 */
	   		if((data & (1U << bit)) != 0U)
	   		{
	   			GPIOA->BSRR = (1UL << 7U);
	   		}
	   		else
	   		{
	   			GPIOA->BSRR = (1UL << (7U + 16U));
	   		}

	   		/* CLK high: OLED reads the MOSI bit */
	   		GPIOB->BSRR = (1UL << 8U);
	   	}

	   	/* Leave CLK low */
	   	GPIOB->BSRR = (1UL << (8U + 16U));
	   }

	   void OLED_Write_Command(uint8_t command)
	   {
	   	/* DC low: command */
	   	GPIOC->BSRR = (1UL << (8U + 16U));

	   	/* CS low: select OLED */
	   	GPIOB->BSRR = (1UL << (4U + 16U));

	   	OLED_Send_Byte(command);

	   	/* CS high: de-select OLED */
	   	GPIOB->BSRR = (1UL << 4U);
	   }

	   void OLED_Write_Data(uint8_t data)
	   {
	   	/* DC high: pixel data */
	   	GPIOC->BSRR = (1UL << 8U);

	   	/* CS low */
	   	GPIOB->BSRR = (1UL << (4U + 16U));

	   	OLED_Send_Byte(data);

	   	/* CS high */
	   	GPIOB->BSRR = (1UL << 4U);
	   }

	   void OLED_Init(void)
	   {
	   	OLED_GPIO_Init();

	   	/* Hardware reset */
	   	GPIOA->BSRR = (1UL << (8U + 16U));
	   	delay_ms(10);

	   	GPIOA->BSRR = (1UL << 8U);
	   	delay_ms(10);

	   	/* Display off */
	   	OLED_Write_Command(0xAEU);

	   	/* Display clock */
	   	OLED_Write_Command(0xD5U);
	   	OLED_Write_Command(0x80U);

	   	/* Multiplex ratio: 64 rows */
	   	OLED_Write_Command(0xA8U);
	   	OLED_Write_Command(0x3FU);

	   	/* Display offset */
	   	OLED_Write_Command(0xD3U);
	   	OLED_Write_Command(0x00U);

	   	/* Start line 0 */
	   	OLED_Write_Command(0x40U);

	   	/* DC-DC control */
	   	OLED_Write_Command(0xADU);
	   	OLED_Write_Command(0x8BU);

	   	/* Segment direction */
	   	OLED_Write_Command(0xA1U);

	   	/* COM scan direction */
	   	OLED_Write_Command(0xC8U);

	   	/* COM pins */
	   	OLED_Write_Command(0xDAU);
	   	OLED_Write_Command(0x12U);

	   	/* Contrast */
	   	OLED_Write_Command(0x81U);
	   	OLED_Write_Command(0x7FU);

	   	/* Precharge */
	   	OLED_Write_Command(0xD9U);
	   	OLED_Write_Command(0x22U);

	   	/* VCOM level */
	   	OLED_Write_Command(0xDBU);
	   	OLED_Write_Command(0x35U);

	   	/* Display RAM controls pixels */
	   	OLED_Write_Command(0xA4U);

	   	/* Normal display */
	   	OLED_Write_Command(0xA6U);

	   	/* Display on */
	   	OLED_Write_Command(0xAFU);
	   }

	   void OLED_Update(void)
	   {
	   	for(int page = 0; page < 8; page++)
	   	{
	   		/* Select page 0 through page 7 */
	   		OLED_Write_Command(0xB0U | page);

	   		/*
	   		 * SH1106 visible display usually begins
	   		 * at internal column 2.
	   		 */
	   		OLED_Write_Command(0x02U);
	   		OLED_Write_Command(0x10U);

	   		for(int x = 0; x < WIDTH; x++)
	   		{
	   			int position = (page * WIDTH) + x;

	   			OLED_Write_Data(FRAME_BUFFER[position]);
	   		}
	   	}
	   }

