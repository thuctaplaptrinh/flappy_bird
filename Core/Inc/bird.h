/*
 * bird.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Asus
 */

#ifndef INC_BIRD_H_
#define INC_BIRD_H_

#include "button.h"
#include "lcd.h"
#include "led_7seg.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/* Constants ----------------------------------------------------------------*/
#define BIRD_X          50
#define BIRD_SIZE       16
#define PIPE_WIDTH      40
#define PIPE_GAP        120
//#define PIPE_SPEED      3
#define GRAVITY         1
#define JUMP_FORCE      -8
#define NUM_PIPES       2

#define SKY_COLOR       LIGHTBLUE  // Light blue
#define BIRD_COLOR     	YELLOW     // Yellow
#define PIPE_COLOR      GREEN      // Green
#define GROUND_COLOR    BROWN      // Brown

/* Functions */
void game_init(void);
void game_process(void);

#endif /* INC_BIRD_H_ */
