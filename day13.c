
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>

typedef enum {POSITION, IMMEDIATE, RELATIVE} mode;
long *param(const mode m, const long relative_base, long Intcode[], const size_t i) {
        switch (m) {
                case POSITION:
                        return &Intcode[Intcode[i]];
                case IMMEDIATE:
                        return &Intcode[i];
                case RELATIVE:
                        return &Intcode[relative_base + Intcode[i]];
                default:
                        puts("something went wrong");
                        exit(EXIT_FAILURE);
        }
}

typedef enum {X_POS, Y_POS, TITLE_ID, HALT} output_type;
typedef struct {
        output_type type;
        long relative_base;
        long output;
        size_t pc;
} program_state;

program_state exec(long Intcode[], const int input, program_state state) {
        state.type = (state.type + 1) % 3;
        bool stop = false;
        size_t i = state.pc;
        while (!stop) {
                const long instruction = Intcode[i];
                const long opcode = instruction % 100;

                // assert(opcode != 3);

                const mode param_mode_1 = instruction % 1000 / 100;
                const mode param_mode_2 = instruction % 10000 / 1000;
                const mode param_mode_3 = instruction / 10000;

                long *const param1 = param(param_mode_1, state.relative_base, Intcode, i+1);
                long *const param2 = param(param_mode_2, state.relative_base, Intcode, i+2);
                long *const param3 = param(param_mode_3, state.relative_base, Intcode, i+3);

                switch (opcode) {
                        case 1:
                                *param3 = *param1 + *param2;
                                i += 4;
                                break;
                        case 2:
                                *param3 = (*param1) * (*param2);
                                i += 4;
                                break;
                        case 3:
                                assert(param_mode_1 != IMMEDIATE);
                                *param1 = input;
                                i += 2;
                                break;
                        case 4:
                                i += 2;
                                state.output = *param1;
                                state.pc = i;
                                return state;
                        case 5:
                                if (*param1) i = *param2;
                                else i += 3;
                                break;
                        case 6:
                                if (!(*param1)) i = *param2;
                                else i += 3;
                                break;
                        case 7:
                                assert(param_mode_3 != IMMEDIATE);
                                if (*param1 < *param2)
                                        *param3 = 1;
                                else
                                        *param3 = 0;
                                i += 4;
                                break;
                        case 8:
                                assert(param_mode_3 != IMMEDIATE);
                                if (*param1 == *param2)
                                        *param3 = 1;
                                else
                                        *param3 = 0;
                                i += 4;
                                break;
                        case 9:
                                state.relative_base += *param1;
                                i += 2;
                                break;

                        case 99:
                                state.type = HALT;
                                stop = true;
                                break;
                        default:
                                printf("Invalid opcode %ld\n", opcode);
                                exit(EXIT_FAILURE);
                }
        }
        return state;
}

int main(void) {
        char *str = 0;
        size_t size;
        getdelim(&str, &size, EOF, stdin);

        const size_t IntcodeSize = size / 2;
        const size_t IntcodeBytes = IntcodeSize * sizeof(long);
        long *Intcode = calloc(IntcodeSize, sizeof(long));
        size_t count = 0;
        char *token = strtok(str, ",");
        while (token) {
                Intcode[count] = atoi(token);
                ++count;
                token = strtok(0, ",");
        }
        free(str);
        long *IntcodeCopy = malloc(IntcodeBytes);
        memcpy(IntcodeCopy, Intcode, IntcodeBytes);

        enum {EMPTY, WALL, BLOCK, HORIZ, BALL};

        size_t block_tile_count = 0;
        program_state state = {0};
        state.type = -1;
        while (state.type != HALT) {
                state = exec(Intcode, 0, state);
                if (state.type == TITLE_ID && state.output == BLOCK) {
                        ++block_tile_count;
                }
        }

        printf("Part One: %zu\n", block_tile_count);


        const char *const type_names[] = {"EMPTY", "WALL", "BLOCK", "HORIZ", "BALL"};
        const Uint32 type_colors[] = {
                [EMPTY] = 0, //black
                [WALL] = 0xFF0000FF, //red
                [BLOCK] = 0xFFFF00A7, //purple
                [HORIZ] = 0xFFFF0000, //blue
                [BALL] = 0xFF00FF00, //green
        };

        SDL_Init(SDL_INIT_VIDEO);

        SDL_Window *win = SDL_CreateWindow("block breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 41*12, 24*12, 0);
        SDL_Renderer *rend = SDL_CreateRenderer(win, -1, 0);
        SDL_Texture *text = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 41, 24);
        // 41 x 24 screen
        bool quit = false;
        Uint32 screen[24][41] = {0};
        int x_pos, y_pos;
        int paddle_x_pos, ball_x_pos, input;
        bool reset = false;
        state.type = -1;
        state.relative_base = 0;
        state.pc = 0;
        IntcodeCopy[0] = 2;
        size_t build_count = 0;
        while (!quit) {
                const Uint8 *keyboard = SDL_GetKeyboardState(0);
                if (build_count < 41*24*3) {
                        state = exec(IntcodeCopy, 0, state);
                } else {
                        input = (ball_x_pos > paddle_x_pos) - (ball_x_pos < paddle_x_pos);
                        state = exec(IntcodeCopy, input, state);
                }

                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_QUIT) {
                                quit = true;
                        }
                }

                switch (state.type) {
                        case X_POS:
                                x_pos = state.output;
                                break;
                        case Y_POS:
                                y_pos = state.output;
                                break;
                        case TITLE_ID:
                                if (x_pos == -1) {
                                        printf("SCORE: %ld\n", state.output);
                                } else {
                                        if (state.output == BALL) {
                                                ball_x_pos = x_pos;
                                        } else if (state.output == HORIZ) {
                                                paddle_x_pos = x_pos;
                                        }
                                        screen[y_pos][x_pos] = type_colors[state.output];
                                } break;
                        case HALT:
                                quit = true;
                                break;
                }

                if (build_count >= 41*24*3) {
                        SDL_RenderClear(rend);
                        SDL_UpdateTexture(text, 0, screen, 41 * 4);
                        SDL_RenderCopy(rend, text, 0, 0);
                        SDL_RenderPresent(rend);
                        SDL_Delay(2);
                } else ++build_count;
        }


        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        free(Intcode);
        free(IntcodeCopy);
}
