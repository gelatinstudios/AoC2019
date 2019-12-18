
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gmp.h>

typedef enum {POSITION, IMMEDIATE, RELATIVE} mode;
mpz_t *param(const mode m, const long relative_base, mpz_t Intcode[], const size_t i) {
        switch (m) {
                case POSITION:
                        return &Intcode[mpz_get_ui(Intcode[i])];
                case IMMEDIATE:
                        return &Intcode[i];
                case RELATIVE:
                        return &Intcode[relative_base + mpz_get_si(Intcode[i])];
                default:
                        puts("something went wrong");
                        exit(EXIT_FAILURE);
        }
}

bool stop;

int exec(mpz_t Intcode[], const long input, size_t *const pc, long *relative_base) {
        for (size_t i = *pc; !stop; ) {
                const long instruction = mpz_get_ui(Intcode[i]);
                const long opcode = instruction % 100;

                const mode param_mode_1 = instruction % 1000 / 100;
                const mode param_mode_2 = instruction % 10000 / 1000;
                const mode param_mode_3 = instruction / 10000;

                mpz_t *const param1 = param(param_mode_1, *relative_base, Intcode, i+1);
                mpz_t *const param2 = param(param_mode_2, *relative_base, Intcode, i+2);
                mpz_t *const param3 = param(param_mode_3, *relative_base, Intcode, i+3);

                // gmp_printf("pc=%zu opcode=%ld\n", i, opcode);

                switch (opcode) {
                        case 1:
                                mpz_add(*param3, *param1, *param2);
                                i += 4;
                                break;
                        case 2:
                                mpz_mul(*param3, *param1, *param2);
                                i += 4;
                                break;
                        case 3:
                                assert(param_mode_1 != IMMEDIATE);
                                mpz_set_si(*param1, input);
                                i += 2;
                                break;
                        case 4:
                                *pc = i + 2;
                                const int output = mpz_get_si(*param1);
                                assert(output == 1 || output == 0);
                                return output;
                        case 5:
                                if (mpz_sgn(*param1)) i = mpz_get_si(*param2);
                                else i += 3;
                                break;
                        case 6:
                                if (!mpz_sgn(*param1)) i = mpz_get_si(*param2);
                                else i += 3;
                                break;
                        case 7:
                                assert(param_mode_3 != IMMEDIATE);
                                if (mpz_cmp(*param1, *param2) < 0)
                                        mpz_set_si(*param3, 1);
                                else
                                        mpz_set_si(*param3, 0);
                                i += 4;
                                break;
                        case 8:
                                assert(param_mode_3 != IMMEDIATE);
                                if (!mpz_cmp(*param1, *param2))
                                        mpz_set_si(*param3, 1);
                                else
                                        mpz_set_si(*param3, 0);
                                i += 4;
                                break;
                        case 9:
                                *relative_base += mpz_get_si(*param1);
                                i += 2;
                                break;

                        case 99:
                                stop = true;
                                return -1;

                        default:
                                printf("Invalid opcode %ld\n", opcode);
                                return -1;
                }
        }
        return -1;
}

typedef struct {
        long x, y;
} coord;

#define MAX_PANELS 10000
typedef struct {
        coord position;
        int color;
} panel;

size_t robot(mpz_t *Intcode, panel *painted_panels, bool first_panel_white) {
        size_t painted_panel_count = 0;

        typedef enum {UP, RIGHT, DOWN, LEFT} robot_orientation;

        typedef struct {
                coord position;
                robot_orientation orientation;
        } robot_state;

        stop = false;
        size_t pc = 0;
        robot_state robot = {0};
        bool output_is_color = true;
        long relative_base = 0;
        while (!stop) {
                int color = 0;
                for (size_t i = 0; i < painted_panel_count; ++i) {
                        if (painted_panels[i].position.x == robot.position.x &&
                            painted_panels[i].position.y == robot.position.y) {
                                color = painted_panels[i].color;
                                break;
                        }
                }
                if (first_panel_white) {
                        color = 1;
                        first_panel_white = false;
                }
                const int output = exec(Intcode, color, &pc, &relative_base);
                if (output_is_color) {
                        bool already_painted = false;
                        for (size_t i = 0; i < painted_panel_count; ++i) {
                                if (painted_panels[i].position.x == robot.position.x &&
                                    painted_panels[i].position.y == robot.position.y) {
                                        painted_panels[i].color = output;
                                        already_painted = true;
                                        break;
                                }
                        }
                        if (!already_painted) {
                                painted_panels[painted_panel_count].position = robot.position;
                                painted_panels[painted_panel_count].color = output;
                                ++painted_panel_count;
                        }

                        assert(painted_panel_count < MAX_PANELS);
                } else {
                        if (!output) {
                                robot.orientation = (robot.orientation - 1 + 4) % 4;
                        } else if (output == 1) {
                                robot.orientation = (robot.orientation + 1) % 4;
                        }

                        switch (robot.orientation) {
                                case UP:
                                        ++robot.position.y;
                                        break;
                                case DOWN:
                                        --robot.position.y;
                                        break;
                                case LEFT:
                                        --robot.position.x;
                                        break;
                                case RIGHT:
                                        ++robot.position.x;
                                        break;
                        }
                }
                output_is_color = !output_is_color;
        }

        return painted_panel_count;
}

int main(void) {
        char *str = 0;
        size_t size;
        getdelim(&str, &size, EOF, stdin);

        const size_t IntcodeSize = size * 512;
        const size_t IntcodeBytes = IntcodeSize * sizeof(mpz_t);
        mpz_t *Intcode = malloc(IntcodeBytes);
        for (size_t i = 0; i < IntcodeSize; ++i) {
                mpz_init_set_si(Intcode[i], 0);
        }
        size_t count = 0;
        char *token = strtok(str, ",");
        while(token) {
                mpz_set_si(Intcode[count], atol(token));
                ++count;
                token = strtok(0, ",");
        }
        free(str);

        mpz_t *IntcodeCopy = malloc(IntcodeBytes);

        memcpy(IntcodeCopy, Intcode, IntcodeBytes);

        panel painted_panels[MAX_PANELS] = {0};

        const size_t painted_panel_count = robot(Intcode, painted_panels, 0);
        printf("Part One: %zu\n", painted_panel_count);

        const size_t painted_panel_count2 = robot(IntcodeCopy, painted_panels, 1);
        printf("Part Two: %zu\n", painted_panel_count2);
        for (int y = -5; y <= 0; ++y) {
                for (int x = 0; x < 64; ++x) {
                        bool found = false;
                        for (size_t i = 0; i < painted_panel_count2; ++i) {
                                if (painted_panels[i].position.x == x &&
                                    painted_panels[i].position.y == y) {
                                            if (painted_panels[i].color) {
                                                found = true;
                                                putchar('#');
                                                break;
                                            }
                                }
                        }
                        if (!found) putchar('.');
                }
                putchar('\n');
        }

        free(Intcode);
        free(IntcodeCopy);
}
