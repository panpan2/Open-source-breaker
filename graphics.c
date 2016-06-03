#include "includes.h"
//TODO: remove
#define BAR_COLOUR 0x2
#define BALL_COLOUR 0x3

// 'global' variables to store screen info
int32_t screenwidth;
int32_t screenheight;
int32_t gamewidth;
int32_t gameheight;
SDL_Window *window;
SDL_DisplayMode DM;
SDL_Renderer *renderer;

/*
 * Initialise screenwidth, height and open frame buffer device
 */ 
void initialise_graphics() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GetCurrentDisplayMode(0, &DM);
    window = SDL_CreateWindow(
            "OPENSOURCE BREAKER",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            DM.w,
            DM.h,
            SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS
    );
    if(window == NULL) {
        fprintf(stderr, "Could not create window %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        free(window);
        fprintf(stderr, "Error in creating renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 1);
    SDL_RenderFillRect(renderer, NULL);
    refresh_screen();
    screenwidth = gamewidth = DM.w;
    screenheight = gameheight = DM.h;
}

void refresh_screen() {
    SDL_RenderPresent(renderer);
}

/*
 * Free any memory assocaited for graphics
 */
void destroy_graphics() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/*
 * Draw background of game, whole window associated
 * with renderer
 */
void draw_background() {
    SDL_SetRenderDrawColor(renderer, BACK_R, BACK_G, BACK_B, BACK_A);
    SDL_RenderDrawRect(renderer, NULL);
}

/*
 * Draw player bar
 */ 
void draw_bar(int32_t x, int32_t y, int32_t width, int32_t height, int32_t colour) {
    SDL_Rect bar;
    bar.x = x;
    bar.y = y;
    bar.w = width;
    bar.h = height;
    //TODO: input colour
    SDL_SetRenderDrawColor(renderer, BAR_R, BAR_G, BAR_B, BAR_A);
    SDL_RenderDrawRect(renderer, &bar);
}

/*
 * Draw a brick at start or when destroyed
 */ 
void draw_brick(int32_t x, int32_t y, int32_t width, int32_t height, int32_t colour) {
    SDL_Rect brick;
    brick.x = x;
    brick.y = y;
    brick.w = width;
    brick.h = height;
    //TODO set renderer COLOUR
    SDL_RenderDrawRect(renderer, &brick);  
}

/*
 * Draw the ball to the background color or to its new position
 */ 
void draw_ball(int32_t x, int32_t y, int32_t radius, int32_t colour) {
    //TODO: check colour
    SDL_SetRenderDrawColor(renderer, BALL_R, BALL_G, BALL_B, BALL_A);
    draw_filled_circle(x, y, radius);
}
 
/*
 * Draw the initial game, used ONLY there
 */ 
void draw_game(bar_t *bar, ball_t *ball, int32_t *bricks) {
    draw_background();
    draw_bar(bar->position.x, bar->position.y, bar->width, bar->height, BAR_COLOUR); 
    for(int32_t i = 0; i < MAX_BRICKS_PER_LEVEL; i++) {
        vector2D_t corner = corner_of_brick(i);
        draw_brick(corner.x, corner.y, BRICK_WIDTH, BRICK_HEIGHT, 
                bricks_level[bar->level][i]);
    }
    draw_ball(ball->position.x, ball->position.y, ball->diameter / 2, BALL_COLOUR);
    refresh_screen();
}

/*
 * Draw a filled circle with center (x0, y0) and the given radius
 */ 
void draw_filled_circle(int32_t x0, int32_t y0, int32_t radius) {
    int32_t x = 0, y = radius;
    int32_t error = 1 - radius;
    do {
        if (error < 0) {
            x++;
            y++;
            error = error + 2 * x + 3;
        } else {
            x++;
            y--;
            error = error + 2 * x - 2 * y + 5;
        }
        for(int32_t y = -radius; y <= radius; y++) {
           for(int32_t x = -radius; x <= radius; x++) {
                if(x * x+ y * y <= radius * radius) {
                    SDL_RenderDrawPoint(renderer,x0 + x,y0 + y);
                }
            }
        }
    } while (x < y);
}

/*
 * Draw screen when winning the game
 */
void draw_win_screen() {
    //TODO:
}

/*
 * Draw the game over screen
 */
void draw_gameover_screen() {
    //TODO
}

/*
 * Color pixel (x, y) with color c
 */
void draw_pixel(SDL_Renderer* renderer, int32_t x, int32_t y) {
    SDL_RenderDrawPoint(renderer, x, y);
}

/*
 * Draw a line from (x0, y0) to (y0, y1)
 */
void draw_line(SDL_Renderer* renderer, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t out = SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
    if (out) {
        fprintf(stderr, "Error when drawing the line\n");
        destroy_graphics();
        exit(EXIT_FAILURE);
    }
}
 
/*
 * Draw a filled circle using the
 * Midpoint circle algorithm
 */
void draw_circle(int32_t x0, int32_t y0, int32_t radius) {
     int32_t x = 0, y = radius;
     int32_t dp = 1 - radius;
     do {
         if (dp < 0) {
             dp = dp + 2 * (++x) + 3;
         } else {
             dp = dp + 2 * (++x) - 2 * (--y) + 5;
         }
         draw_pixel(renderer, x0 + x, y0 + y);
         draw_pixel(renderer, x0 - x, y0 + y);
         draw_pixel(renderer, x0 + x, y0 - y);
         draw_pixel(renderer, x0 - x, y0 - y);
         draw_pixel(renderer, x0 + y, y0 + x);
         draw_pixel(renderer, x0 - y, y0 + x);
         draw_pixel(renderer, x0 + y, y0 - x);
         draw_pixel(renderer, x0 - y, y0 - x);
     } while (x < y);
 }
