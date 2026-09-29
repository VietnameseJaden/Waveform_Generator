/*
 * Driver.h
 *
 *  Created on: Sep 10, 2026
 *      Author: thisf
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define WIDTH 128
#define HEIGHT 64
#define WAVE_WIDTH 128
#define WAVE_HEIGHT 64
#define SIZE (WIDTH * HEIGHT) / 8

extern uint8_t FRAME_BUFFER[SIZE];
void position(int x, int y, bool state);
bool get_position(int x, int y);
void fill_row(int y, bool state);
void draw_image(int x, int y, int height,
		int width, const uint8_t *image);
void draw_wave(int y);
void draw_line(int lastx, int lasty, int x1, int y1);
void clearbuffer();
void OLED_GPIO_Init(void);
void OLED_Init(void);
void OLED_Send_Byte(uint8_t data);
void OLED_Write_Command(uint8_t command);
void OLED_Write_Data(uint8_t data);
void OLED_Update(void);
void delay_ms(uint32_t delay);


#endif /* INC_DISPLAY_H_ */
