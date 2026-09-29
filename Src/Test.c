/*
 * Test.c
 *
 *  Created on: Sep 12, 2026
 *      Author: thisf
 */

#include "Display.h"
#include "main.h"
#include "kuromi.h"
#include <stdio.h>

int main() {
	while(1){
		draw_image(0, 0, KUROMI_HEIGHT, KUROMI_WIDTH, kuromi);
		draw_image(40, 0, KUROMI_HEIGHT, KUROMI_WIDTH, kuromi);
		for(int y = 0; y < KUROMI_HEIGHT; y++)
		{
			for(int x = 0; x < KUROMI_WIDTH; x++)
			{
				if(get_position(x,y))
				{
					printf("1");
				}
				else
				{
					printf("0");
				}
			}
			printf("\n");
		}
		OLED_Init();

		draw_image(0, 0, KUROMI_HEIGHT, KUROMI_WIDTH, kuromi);

		OLED_Update();
}
}

