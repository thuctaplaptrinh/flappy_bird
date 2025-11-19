/*
 * bird.c (FSM version)
 * Refactor by ChatGPT — 2025-11-11
 */

#include "bird.h"

/* Struct --------------------------------------------------------------------*/
typedef struct {
    int y, y_pre;
    int velocity;
} Bird;

typedef struct {
    int x, x_pre;
    int gap_y;
} Pipe;

/* Enums ---------------------------------------------------------------------*/
typedef enum {
    ST_MENU,
    ST_PLAYING,
    ST_PAUSED,
    ST_GAME_OVER
} GameState;

typedef enum {
    EV_NONE,
    EV_BTN_UP,
    EV_COLLISION,
    EV_TICK
} GameEvent;

/* Globals -------------------------------------------------------------------*/
static Bird bird;
static Pipe pipes[NUM_PIPES];
int PIPE_SPEED = 3;
static int score = 0;
static GameState state = ST_MENU;

/* Function prototypes -------------------------------------------------------*/
static void bird_draw(int y, uint16_t color);
static void pipes_draw(void);
static void pipes_update(void);
static void game_draw(void);
static void game_init_scene(void);
static void game_start(void);
static void game_pause(void);
static void game_resume(void);
static void game_over_screen(void);
static void fsm_dispatch(GameEvent ev);
static GameEvent get_event(void);

//extern uint8_t button_count[4];  // giả sử phần đọc nút nằm nơi khác

/* -------------------------------------------------------------------------- */
/*  FSM IMPLEMENTATION                                                        */
/* -------------------------------------------------------------------------- */

void game_init(void) {
    lcd_clear(SKY_COLOR);
    lcd_fill(0, 220, 319, 239, GROUND_COLOR);
    lcd_draw_rectangle(0, 220, 319, 239, 0x0000);

    lcd_show_string_center(0, 100, "FLAPPY BIRD", 0xFFFF, SKY_COLOR, 24, 0);
    lcd_show_string(20, 250, "Score: 0", 0xFFFF, SKY_COLOR, 16, 0);
    lcd_show_string_center(0, 150, "Press UP to START", 0xFFFF, SKY_COLOR, 16, 0);

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
	    pipes[i].gap_y = 70 + rand() % 80;  // Gap random 70-150
	}

    score = 0;
    state = ST_MENU;
}

/* Gọi trong vòng lặp chính */
void game_process(void) {
    static uint8_t counter = 0;
    counter = (counter + 1) % 2;

    GameEvent ev = get_event();
    fsm_dispatch(ev);

    if (state == ST_PLAYING && counter == 0) {
        // update logic
        bird.velocity += GRAVITY;
        bird.y_pre = bird.y;
        bird.y += bird.velocity;

        pipes_update();

        // check collision
        if (bird.y > 204 || bird.y < 0) {
            fsm_dispatch(EV_COLLISION);
            return;
        }

        for (int i = 0; i < NUM_PIPES; i++) {
            if (bird.y < pipes[i].gap_y - PIPE_GAP / 2 ||
                bird.y + BIRD_SIZE > pipes[i].gap_y + PIPE_GAP / 2) {
                if (BIRD_X + BIRD_SIZE > pipes[i].x &&
                    BIRD_X < pipes[i].x + PIPE_WIDTH) {
                    fsm_dispatch(EV_COLLISION);
                    return;
                }
            }

            if (pipes[i].x + PIPE_WIDTH < BIRD_X &&
                pipes[i].x + PIPE_WIDTH >= BIRD_X - PIPE_SPEED) {
                score++;
                if (score%10 == 5) {
                    PIPE_SPEED += 2;
                }
            }
        }

        game_draw();
    }
}

/* -------------------------------------------------------------------------- */
/*  FSM dispatcher                                                            */
/* -------------------------------------------------------------------------- */
static void fsm_dispatch(GameEvent ev) {
    switch (state) {
    case ST_MENU:
        if (ev == EV_BTN_UP) {
            game_start();
        }
        break;

    case ST_PLAYING:
        if (ev == EV_BTN_UP) {
            bird.velocity = JUMP_FORCE;
        } else if (ev == EV_COLLISION) {
            game_over_screen();
        }
        break;

    case ST_GAME_OVER:
        if (ev == EV_BTN_UP) {
            game_init_scene();
            state = ST_MENU;
        }
        break;

    case ST_PAUSED:
        if (ev == EV_BTN_UP) {
            game_resume();
        }
        break;

    default:
        break;
    }
}

/* -------------------------------------------------------------------------- */
/*  Event source                                                              */
/* -------------------------------------------------------------------------- */
static GameEvent get_event(void) {
    if (button_count[0] == 1) {   // giả sử UP là button_count[0]
        button_count[0] = 0;
        return EV_BTN_UP;
    }
    return EV_NONE;
}

/* -------------------------------------------------------------------------- */
/*  Game State actions                                                        */
/* -------------------------------------------------------------------------- */

static void game_init_scene(void) {
    lcd_clear(SKY_COLOR);
    lcd_fill(0, 220, 319, 239, GROUND_COLOR);
    lcd_draw_rectangle(0, 220, 319, 239, 0x0000);
    lcd_show_string_center(0, 100, "FLAPPY BIRD", 0xFFFF, SKY_COLOR, 24, 0);
    lcd_show_string(20, 250, "Score: 0", 0xFFFF, SKY_COLOR, 16, 0);
    lcd_show_string_center(0, 150, "Press UP to START", 0xFFFF, SKY_COLOR, 16, 0);

    // reset bird & pipes
    bird.y = 120;
    bird.y_pre = bird.y;
    bird.velocity = 0;
    for (int i = 0; i < NUM_PIPES; i++) {
        pipes[i].x = 150 + i * 140;
        pipes[i].gap_y = 70 + rand() % 80;
    }
    score = 0;
}

static void game_start(void) {
    state = ST_PLAYING;
    lcd_clear(SKY_COLOR);
    lcd_fill(0, 220, 319, 239, GROUND_COLOR);
}

static void game_pause(void) {
    state = ST_PAUSED;
    lcd_show_string_center(0, 120, "PAUSED", 0xFFFF, SKY_COLOR, 24, 0);
}

static void game_resume(void) {
    state = ST_PLAYING;
    lcd_fill(0, 100, 240, 140, SKY_COLOR);
}

static void game_over_screen(void) {
    state = ST_GAME_OVER;
    lcd_fill(80, 80, 240, 160, 0xF800);
    lcd_show_string_center(0, 100, "GAME OVER", 0xFFFF, 0xF800, 24, 0);
    lcd_show_string_center(0, 130, "Score:", 0xFFFF, 0xF800, 20, 0);
    char buf[20];
    snprintf(buf, sizeof buf, "%d", score);
    lcd_show_string_center(0, 155, buf, 0xFFFF, 0xF800, 20, 0);
    lcd_show_string_center(0, 190, "UP to Restart", 0xFFFF, 0xF800, 16, 0);
}

/* -------------------------------------------------------------------------- */
/*  Drawing / Update helpers                                                  */
/* -------------------------------------------------------------------------- */

static void bird_draw(int y, uint16_t color) {
    int x = BIRD_X;
    lcd_draw_circle(x + 8, y + 8, color, BIRD_SIZE / 2, 1);
}

static void pipes_draw(void) {
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

static void pipes_update(void) {
    for (int i = 0; i < NUM_PIPES; i++) {
        pipes[i].x_pre = pipes[i].x;
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x < -PIPE_WIDTH) {
            lcd_fill(0, 0, pipes[i].x_pre + PIPE_WIDTH, pipes[i].gap_y - PIPE_GAP/2, SKY_COLOR);
        	lcd_fill(0, pipes[i].gap_y + PIPE_GAP/2, pipes[i].x_pre + PIPE_WIDTH, 219, SKY_COLOR);
            pipes[i].x = 240;
            pipes[i].gap_y = 70 + rand() % 80;
        }
    }
}

static void game_draw(void) {
    bird_draw(bird.y_pre, SKY_COLOR);
    bird_draw(bird.y, BIRD_COLOR);
    pipes_draw();

    char buf[20];
    snprintf(buf, sizeof buf, "Score: %d", score);
    lcd_fill(20, 250, 150, 270, SKY_COLOR);
    lcd_show_string(20, 250, buf, 0xFFFF, SKY_COLOR, 16, 0);
}
