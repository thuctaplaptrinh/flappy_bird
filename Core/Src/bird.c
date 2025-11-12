/*
 * bird.c
 *
 *  Created on: Nov 7, 2025
 *      Author: Asus
 */

/* Includes ------------------------------------------------------------------*/
#include "bird.h"

/* Struct --------------------------------------------------------------------*/
typedef struct BIRD {
    int y, y_pre;
    int velocity;
} S_BIRD;

typedef struct PIPE {
    int x, x_pre;
    int gap_y;  // Vị trí gap ngẫu nhiên
} S_PIPE;

/* Globals -------------------------------------------------------------------*/
S_BIRD bird;
S_PIPE pipes[NUM_PIPES];
int score = 0;
int game_state = 0;  // 0: menu, 1: playing, 2: over

/* Private Objects -----------------------------------------------------------*/

// Bird object
void bird_draw(int y, uint16_t color);
void bird_direction_process(void);
void bird_moving_process(void);

// Pipe object
void pipes_draw(void);
void pipes_update(void);

// Game Engine object
void game_draw(void);
void game_handler(void);
void game_over(void);

/* Declare Private Support Functions -----------------------------------------*/
uint8_t is_button_up(void);
//uint8_t is_button_down(void);
//uint8_t is_button_left(void);
//uint8_t is_button_right(void);

/* Public Functions ----------------------------------------------------------*/
/**
 * @brief  	Init Pac-Man game
 * @param  	None
 * @note  	Call when you want to init game
 * @retval 	None
 */
void game_init(void) {
	/*
	 * DONE (can be modified)
	 *
	 * 1. Draw a frame for the maze
	 */
	lcd_clear(SKY_COLOR);
	// draw ground
	lcd_fill(0, 220, 319, 239, GROUND_COLOR);
	lcd_draw_rectangle(0, 220, 319, 239, 0x0000);  // black frame

	// Title & score
	lcd_show_string_center(0, 100, "FLAPPY BIRD", 0xFFFF, SKY_COLOR, 24, 0);
	lcd_show_string(20, 250, "Score: 0", 0xFFFF, SKY_COLOR, 16, 0);
	lcd_show_string_center(0, 150, "Press UP to START", 0xFFFF, SKY_COLOR, 16, 0);

	/*
	 * TO DO
	 *
	 * 3. Init Bird object.
	 * - Firstly, you have to initialize default values for the bird object.
	 */
	bird.y = 120;
	bird.y_pre = bird.y;
	bird.velocity = 0;

	/*
	 * TO DO
	 *
	 * 4. Init Pipes object.
	 * - Firstly, you have to initialize default values for the pipe objects.
	 */
	for (int i = 0; i < NUM_PIPES; i++) {
	    pipes[i].x = 150 + i * 140;
	    pipes[i].gap_y = 70 + rand() % 90;  // Gap random 70-150
	}

	score = 0;
	game_state = 0;

}

/**
 * @brief  	Process game
 * @param  	None
 * @note  	Call in loop (main)
 * @retval 	None
 */
void game_process(void) {
	static uint8_t counter_game = 0;
	counter_game = (counter_game + 1) % 2;

//	pacman_direction_process(); // Put this function here to read buttons.

	if (game_state == 0) {  // Menu: chờ start
	    if (is_button_up()) {
	        game_state = 1;
	        lcd_clear(SKY_COLOR);
	        lcd_fill(0, 220, 319, 239, GROUND_COLOR);
	    }
	    return;
	}

	// Jump input (mỗi frame kiểm tra)
	bird_direction_process();

	if (counter_game == 0) {
		bird_moving_process();
		pipes_update();

		game_handler();
		game_draw();
	}
}

/* Private Functions ---------------------------------------------------------*/
void game_draw(void) {
	/*
	 * TO DO
	 *
	 * Draw Bird and Pipes.
	 *
	 * Hint: Remember to delete the object in the previous position, before drawing the new one.
	 */
	// Xóa bird cũ
	bird_draw(bird.y_pre, SKY_COLOR);

	// Vẽ mới
	bird_draw(bird.y, BIRD_COLOR);
	pipes_draw();

	// Cập nhật score (cho score hiển thị ra led_7seg)
	char buf[20];
	sprintf(buf, "Score: %d", score);
	lcd_fill(20, 250, 150, 270, SKY_COLOR);
	lcd_show_string(20, 250, buf, 0xFFFF, SKY_COLOR, 16, 0);
}

void game_handler(void) {
	/*
	 * TO DO
	 *
	 * 1. Check the loss condition, show something, and restart the game.
	 * 2. Check the win condition, show something, and restart the game.
	 * 3. Check if Bird has passed through gap between 2 pipes, then update the score.
	 */
	// Game over: chạm ground/top/pipe
	if (bird.y > 205 || bird.y < 0) {
	// chạm nền         chạm trần
	    game_over();
	    return;
	}

	for (int i = 0; i < NUM_PIPES; i++) {
	    if (bird.y < pipes[i].gap_y - PIPE_GAP/2 || bird.y + BIRD_SIZE > pipes[i].gap_y + PIPE_GAP/2) {
	        if (BIRD_X + BIRD_SIZE > pipes[i].x && BIRD_X < pipes[i].x + PIPE_WIDTH) {
	            game_over();
	            return;
	        }
	    }
	    // Score khi qua pipe
	    if (pipes[i].x + PIPE_WIDTH < BIRD_X && pipes[i].x + PIPE_WIDTH >= BIRD_X - PIPE_SPEED) {
	        score++;
	    }
	}
}

void bird_direction_process(void) {
	/*
	 * TO DO
	 *
	 * Let user use button to control Bird.
	 */
	if (is_button_up()) {
		 bird.velocity = JUMP_FORCE;
	}
}

void bird_moving_process(void) {
	/*
	 * TO DO
	 *
	 * Update Bird's current and previous position based on current direction.
	 *
	 */
	bird.velocity += GRAVITY; // di xuong
	bird.y_pre = bird.y;
	bird.y += bird.velocity; // +JUMP_FORECE di xuong
}

void game_over(void) {
    game_state = 2;
    lcd_fill(60, 80, 180, 160, 0xF800);  // Nền đỏ
    lcd_show_string_center(-20, 100, "GAME OVER", 0xFFFF, 0xF800, 24, 0);
    lcd_show_string_center(0, 130, "Score:", 0xFFFF, 0xF800, 20, 0);
    char buf[20];
//    sprintf(buf, "%d", score);
    lcd_show_string_center(0, 155, buf, 0xFFFF, 0xF800, 20, 0);
    lcd_show_string_center(0, 190, "UP to Restart", 0xFFFF, 0xF800, 16, 0);

    HAL_Delay(1000);
    while (!is_button_up());  // Chờ nhấn
    game_init();
}

void pipes_update(void) {
    for (int i = 0; i < NUM_PIPES; i++) {
    	pipes[i].x_pre = pipes[i].x;
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x + PIPE_WIDTH < 0) {
            pipes[i].x = 240;
            pipes[i].gap_y = 70 + rand() % 80;
            /* random gap_y từ 70 đến 150 vì gap_y nằm giữa khe (khe rộng 120),
            nên chia ra phần trên là 60 và phần dưới là 60. Vì vậy nên chiều cao
            của ống trên phải lớn hơn hoặc bằng 60 và ống dưới phải bé hơn hoặc bằng
            160 (220 là mặt đất) */
        }
    }
}

void pipes_draw(void) {
	/*
	* TO DO
	*
	* Draw whatever you like
	*/
    for (int i = 0; i < NUM_PIPES; i++) {
    	if (pipes[i].x >= 0) {
			// Pipe trên
			lcd_fill(pipes[i].x_pre, 0, pipes[i].x_pre + PIPE_WIDTH, pipes[i].gap_y - PIPE_GAP/2, SKY_COLOR);
			lcd_fill(pipes[i].x, 0, pipes[i].x + PIPE_WIDTH, pipes[i].gap_y - PIPE_GAP/2, PIPE_COLOR);
			// Pipe dưới
			lcd_fill(pipes[i].x_pre, pipes[i].gap_y + PIPE_GAP/2, pipes[i].x_pre + PIPE_WIDTH, 219, SKY_COLOR);
			lcd_fill(pipes[i].x, pipes[i].gap_y + PIPE_GAP/2, pipes[i].x + PIPE_WIDTH, 219, PIPE_COLOR);
    	}
    	else {
    		// Pipe trên
    		lcd_fill(0, 0, pipes[i].x_pre + PIPE_WIDTH, pipes[i].gap_y - PIPE_GAP/2, SKY_COLOR);
    		lcd_fill(0, 0, pipes[i].x + PIPE_WIDTH, pipes[i].gap_y - PIPE_GAP/2, PIPE_COLOR);
    		// Pipe dưới
            lcd_fill(0, pipes[i].gap_y + PIPE_GAP/2, pipes[i].x_pre + PIPE_WIDTH, 219, SKY_COLOR);
            lcd_fill(0, pipes[i].gap_y + PIPE_GAP/2, pipes[i].x + PIPE_WIDTH, 219, PIPE_COLOR);
    	}
    }
}

void bird_draw(int y, uint16_t color) {
	/*
	* TO DO
	*
	* Draw whatever you like
	*/
    int x = BIRD_X;
    lcd_draw_circle(x + 8, y + 8, color, BIRD_SIZE/2, 1);  // Thân chim
//    lcd_fill(x + 12, y + 4, x + 18, y + 12, color);       // Cánh
}

uint8_t is_button_up(void) {
	/*
	 * TO DO
	 */
	if(button_count[0] == 1) {
		button_count[0] = 0;
		return 1;
	}
	return 0;
}

//uint8_t is_button_down(void) {
//	/*
//	 * TO DO
//	 */
//
//}
//
//uint8_t is_button_left(void) {
//	/*
//	 * TO DO
//	 */
//
//}
//
//uint8_t is_button_right(void) {
//	/*
//	 * TO DO
//	 */
//
//}
